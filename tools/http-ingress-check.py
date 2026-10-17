"""Bounded HTTP ingress regression. Run only on an idle QC unit.

Deliberately holds incomplete/idle connections for <=2s; one incomplete,
unauthenticated heartbeat POST. No valid mutation/output/QC test command.
Before/after exact serial and output OFF guards. Old synchronous parser may
timeout this check; always close all sockets. Never scans the LAN.
"""
import argparse
import concurrent.futures
import json
import socket
import time
import urllib.parse
import urllib.error
import urllib.request


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--url', required=True)
    p.add_argument('--serial', required=True)
    a = p.parse_args()
    u = urllib.parse.urlsplit(a.url)
    assert u.scheme == 'http' and u.hostname and u.path in ('', '/')
    def get(path='/api/health'):
        t = time.monotonic()
        with urllib.request.urlopen(a.url.rstrip('/') + path, timeout=5) as r:
            data = json.load(r)
        assert (data.get('serial') or data.get('hardware', {}).get('serial')) == a.serial
        assert data.get('outputActive') is False
        return round(1000*(time.monotonic()-t), 1), data
    before = get()[1]
    results = {}
    held = []
    try:
        for payload in [b'', b'GET /api/health HTTP/1.1\r\nHost: unfinished',
                        b'POST /api/heartbeat HTTP/1.1\r\nHost: qc\r\nContent-Length: 1000\r\n\r\ntoken=invalid']:
            s = socket.create_connection((u.hostname, u.port or 80), timeout=3)
            held.append(s)
            if payload:
                s.sendall(payload)
        time.sleep(.2)
        results['healthWithThreeIncompleteClientsMs'] = get()[0]
        results['stateWithThreeIncompleteClientsMs'] = get('/api/state')[0]
    finally:
        for s in held:
            s.close()
    with socket.create_connection((u.hostname, u.port or 80), timeout=3) as s:
        s.sendall(b'GET /api/health HTTP/1.1\r\nHost: qc')
        time.sleep(.35)
        s.sendall(b'\r\nConnection: close\r\n\r\n')
        s.settimeout(5)
        raw = b''
        while True:
            chunk = s.recv(4096)
            if not chunk:
                break
            raw += chunk
        assert raw.startswith(b'HTTP/1.1 200'), raw[:80]
        assert a.serial.encode() in raw
        results['fragmentedValidGet'] = 'OK'
    with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:
        results['concurrentGetMs'] = list(pool.map(lambda _: get()[0], range(9)))
    # A client that stops reading a large page used to hold NetworkClient.write.
    with socket.socket() as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1024)
        s.settimeout(3)
        s.connect((u.hostname,u.port or 80))
        s.sendall(b'GET / HTTP/1.1\r\nHost: qc\r\nConnection: close\r\n\r\n')
        time.sleep(.1)
        results['healthDuringSlowReaderMs'] = get()[0]
    try:
        req=urllib.request.Request(a.url.rstrip('/')+'/api/heartbeat',data=b'token=invalid')
        urllib.request.urlopen(req,timeout=5)
        raise AssertionError('Invalid token was accepted')
    except urllib.error.HTTPError as e:
        assert e.code==403, e.code
        results['completePostAuthentication'] = '403 as expected; no mutation'
    after = get()[1]
    assert after['uptime'] >= before['uptime'], 'Unexpected reboot'
    assert after['wifi']['disconnects'] == before['wifi']['disconnects'], 'WiFi changed during check'
    results.update(serial=a.serial, version=after['version'], http=after.get('http'),
                   wifi=after['wifi'], maxLoopMs=after['maxLoopMs'])
    assert max(results['concurrentGetMs'] + [results['healthWithThreeIncompleteClientsMs'],
               results['stateWithThreeIncompleteClientsMs'],results['healthDuringSlowReaderMs']]) < 2000, results
    print(json.dumps(results))


if __name__ == '__main__':
    main()
