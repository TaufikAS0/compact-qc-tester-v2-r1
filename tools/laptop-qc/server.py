"""Local-only laptop QC service. Never opens serial or contacts ESP until Start."""
import csv
import io
import json
import secrets
import threading
import time
import urllib.parse
import urllib.request
import ipaddress
from datetime import datetime, timezone
from pathlib import Path
from http.server import ThreadingHTTPServer, BaseHTTPRequestHandler
from protocol import exercise

ROOT = Path(__file__).parent
LOCK = threading.Lock()
CANCEL = threading.Event()
LOCAL_TOKEN = secrets.token_hex(24)
STATE = {'active': False, 'status': 'IDLE', 'completed': 0, 'detail': 'Belum ada pengujian hardware', 'report': None}

def snapshot():
    with LOCK:
        return json.loads(json.dumps(STATE))

def update(**kwargs):
    with LOCK:
        STATE.update(kwargs)

def device_url(value):
    p = urllib.parse.urlsplit(value.strip())
    if p.scheme != 'http' or p.username or p.password or p.query or p.fragment or p.path not in ('', '/'):
        raise ValueError('Gunakan http://IP-device tanpa path/login')
    ip = ipaddress.ip_address(p.hostname or '')
    if not ip.is_private or ip.is_loopback or ip.is_unspecified or ip.is_multicast:
        raise ValueError('IP harus alamat LAN/AP device')
    if p.port not in (None, 80):
        raise ValueError('Port device harus 80')
    return value.rstrip('/')

def request(base, path, args=None):
    data = urllib.parse.urlencode(args).encode() if args is not None else None
    # Direct device only; do not route through environment proxies.
    opener = urllib.request.build_opener(urllib.request.ProxyHandler({}))
    with opener.open(urllib.request.Request(base + path, data=data), timeout=5) as r:
        result = json.loads(r.read(128 * 1024))
    if result.get('error') or result.get('ok') is False:
        raise RuntimeError(result.get('error', result.get('detail', 'Device rejected request')))
    return result

def run_test(settings):
    report = {'schema': 'COMPACT-SERIAL-1', 'startedUTC': datetime.now(timezone.utc).isoformat(),
              'status': 'INCOMPLETE', 'completed': 0, **settings}
    base = settings['device']; token = None; armed = False
    try:
        import serial
        before = request(base, '/api/state')
        if not before.get('qc', {}).get('profileSelected'):
            raise RuntimeError('Pilih M0/M1 pada dashboard ESP dahulu; laptop tidak mereset sesi QC')
        token = before['token']
        report.update(serialNumber=before['hardware']['serial'], variant=before['qc']['variant'],
                      sessionId=before['qc']['sessionId'], firmware=before['version'], buildUTC=before.get('buildUTC'))
        # Open adapter before arming; invalid/busy COM must not leave an armed device.
        with serial.Serial(settings['port'], settings['baud'], timeout=.05, write_timeout=2,
                           bytesize=8, parity='N', stopbits=1, rtscts=False, dsrdtr=False, xonxoff=False) as port:
            port.reset_input_buffer(); port.reset_output_buffer()
            if CANCEL.is_set(): raise InterruptedError('Cancelled')
            arm = request(base, '/api/serial/start', dict(token=token, mode=settings['mode'], baud=settings['baud'],
                          packets=settings['packets'], timeoutMs=settings['timeoutMs']))
            armed = True
            run = int(arm['runId'], 16); report['runId'] = arm['runId']
            update(status='TESTING', detail='Verifikasi pola/CRC/urutan dua arah', report=report.copy())
            def progress(done, latency):
                report['completed'] = done
                update(completed=done, latencyMs=round(latency, 2), report=report.copy())
            times = exercise(port, run, settings['packets'], settings['timeoutMs'] / 1000, CANCEL, progress)
            report.update(meanMs=round(sum(times)/len(times), 3), maxMs=round(max(times), 3))
            # Device drains final reply non-blocking, then records result.
            deadline = time.monotonic() + 8
            while True:
                if CANCEL.is_set(): raise InterruptedError('Cancelled')
                final = request(base, '/api/state')
                if final.get('token') != token or final.get('qc', {}).get('sessionId') != report['sessionId']:
                    raise RuntimeError('Device restarted/QC session changed during test')
                evidence = final.get('qc', {}).get('serial', {})
                if evidence.get('runId') != arm['runId']:
                    raise RuntimeError('Device result belongs to different test')
                if not evidence.get('active'):
                    if evidence.get('status') != 'PASS' or evidence.get('completed') != settings['packets']:
                        raise RuntimeError('Device did not PASS: ' + evidence.get('detail', 'missing evidence'))
                    break
                if time.monotonic() > deadline:
                    raise TimeoutError('Device final result timeout')
                CANCEL.wait(.25)
            report.update(status='PASS', deviceResult=evidence, qcCode=final['qc'].get('qcCode'))
    except InterruptedError as exc:
        report.update(status='INCOMPLETE', error=str(exc))
    except Exception as exc:
        report.update(status='FAIL', error=str(exc), diagnosis='Test failed; does not isolate adapter/cable/device hardware fault')
    finally:
        if armed and report['status'] != 'PASS':
            try: request(base, '/api/serial/stop', {'token': token})
            except Exception: report['cleanupWarning'] = 'Cannot stop device via network; firmware deadline remains active'
        report['finishedUTC'] = datetime.now(timezone.utc).isoformat()
        folder = ROOT / 'reports'; folder.mkdir(exist_ok=True)
        name = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S') + '-' + secrets.token_hex(4) + '.json'
        try:
            (folder / name).write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding='utf-8')
            report['savedFile'] = name
        except OSError as exc: report['saveError'] = str(exc)
        update(active=False, status=report['status'], detail=report.get('error', 'Seluruh paket cocok dua arah'), report=report)

def settings_from(data):
    cfg = {'device': device_url(str(data.get('device', ''))), 'port': str(data.get('port', '')),
           'mode': str(data.get('mode', 'RS232')), 'baud': int(data.get('baud', 9600)), 'packets': int(data.get('packets', 100)),
           'timeoutMs': int(data.get('timeoutMs', 2000))}
    if cfg['mode'] not in ('RS232','RS485'): raise ValueError('Mode harus RS232/RS485')
    if not cfg['port'] or len(cfg['port']) > 64: raise ValueError('Pilih port adapter')
    if cfg['baud'] not in (1200,2400,4800,9600,19200,38400,57600,115200): raise ValueError('Baud tidak valid')
    if not 10 <= cfg['packets'] <= 1000 or not 500 <= cfg['timeoutMs'] <= 10000: raise ValueError('Batas paket/timeout tidak valid')
    # At 1200 baud a full 128-byte round trip takes >2s.
    if cfg['timeoutMs'] < (2 * 142 + 2 * 14) * 10000 / cfg['baud'] + 300:
        raise ValueError('Timeout terlalu pendek untuk baud/payload ini')
    return cfg

def report_csv(report):
    buf = io.StringIO(newline=''); writer = csv.writer(buf)
    keys = list(report)
    def safe(v):
        s = json.dumps(v, ensure_ascii=False) if isinstance(v, (dict, list)) else str(v)
        return "'" + s if s.lstrip().startswith(('=', '+', '-', '@')) else s
    writer.writerow(keys); writer.writerow([safe(report[k]) for k in keys])
    return ('\ufeff' + buf.getvalue()).encode('utf-8')

class Handler(BaseHTTPRequestHandler):
    def log_message(self, *_): pass
    def send(self, code, data, kind='application/json'):
        if not isinstance(data, bytes): data = json.dumps(data, ensure_ascii=False).encode()
        self.send_response(code); self.send_header('Content-Type', kind)
        self.send_header('Cache-Control', 'no-store'); self.send_header('Content-Length', str(len(data)))
        self.end_headers(); self.wfile.write(data)
    def valid_host(self):
        return self.headers.get('Host') in (f'127.0.0.1:{self.server.server_port}', f'localhost:{self.server.server_port}')
    def do_GET(self):
        if not self.valid_host(): return self.send(403, {'error':'Invalid Host'})
        if self.path == '/': return self.send(200, (ROOT/'index.html').read_bytes(), 'text/html; charset=utf-8')
        if self.path == '/api/state': return self.send(200, {**snapshot(), 'localToken': LOCAL_TOKEN})
        if self.path == '/api/ports':
            try:
                from serial.tools import list_ports
                return self.send(200, {'ports':[{'port':p.device,'description':p.description} for p in list_ports.comports()]})
            except ImportError: return self.send(503, {'error':'Install dependencies: python -m pip install -r requirements.txt'})
        if self.path in ('/report.json','/report.csv'):
            report = snapshot()['report']
            if not report: return self.send(409, {'error':'Belum ada report'})
            return self.send(200, report_csv(report) if self.path.endswith('csv') else report,
                             'text/csv; charset=utf-8' if self.path.endswith('csv') else 'application/json')
        self.send(404, {'error':'Not found'})
    def do_POST(self):
        if not self.valid_host(): return self.send(403, {'error':'Invalid Host'})
        try:
            length = int(self.headers.get('Content-Length', 0))
            if not 0 < length <= 4096: raise ValueError('Request size invalid')
            data = json.loads(self.rfile.read(length))
            if not secrets.compare_digest(str(data.pop('localToken', '')), LOCAL_TOKEN):
                return self.send(403, {'error':'Invalid local session'})
            if self.path == '/api/stop': CANCEL.set(); return self.send(200, {'ok':True})
            if self.path != '/api/start': return self.send(404, {'error':'Not found'})
            cfg = settings_from(data)
            with LOCK:
                if STATE['active']: return self.send(409, {'error':'Test masih berjalan'})
                CANCEL.clear(); STATE.update(active=True,status='STARTING',completed=0,detail='Membuka adapter',report=None)
            threading.Thread(target=run_test, args=(cfg,), daemon=True).start()
            self.send(200, {'ok':True})
        except (ValueError, TypeError) as exc: self.send(400, {'error':str(exc)})

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(); parser.add_argument('--port', type=int, default=8770)
    args = parser.parse_args()
    print(f'Laptop QC: http://127.0.0.1:{args.port} (Ctrl+C to stop). No serial opened automatically.', flush=True)
    try: ThreadingHTTPServer(('127.0.0.1', args.port), Handler).serve_forever()
    except KeyboardInterrupt: CANCEL.set()
