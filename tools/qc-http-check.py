"""Bounded GET-only QC page responsiveness check; never persists full state or tokens."""
import argparse
import datetime as dt
import json
from pathlib import Path
import time
import urllib.request


def check(base, path, expected_serial):
    started = time.monotonic()
    result = {'utc': dt.datetime.now(dt.timezone.utc).isoformat(), 'path': path}
    try:
        with urllib.request.urlopen(base + path, timeout=5) as response:
            raw = response.read(512001)
        if len(raw) > 512000:
            raise ValueError('Response exceeded512KB bound')
        data = json.loads(raw)
        serial = data.get('serial') or data.get('hardware', {}).get('serial')
        result['serial'] = serial
        if serial != expected_serial:
            result['identityMismatch'] = True
            raise ValueError('Unit identity differs; do not continue probing replacement unit')
        result.update(ok=True, bytes=len(raw), **{key: data.get(key) for key in
                      ('version', 'buildUTC', 'uptime', 'resetReason', 'freeHeap', 'minFreeHeap',
                       'maxLoopMs', 'outputActive')})
        wifi = data.get('wifi', {})
        result['wifi'] = {key: wifi.get(key) for key in
                          ('connected', 'rssiDBm', 'disconnects', 'gotIPs', 'lastDisconnectReason')}
    except Exception as error:
        result.update(ok=False, error=str(error)[:240])
    result['rttMs'] = round((time.monotonic() - started) * 1000, 1)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--url', required=True)
    parser.add_argument('--serial', required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    if not args.url.startswith(('http://', 'https://')):
        parser.error('Explicit http(s) URL required')
    probes = []
    for path in ['/api/health', '/api/state', '/api/state', '/api/state', '/api/health']:
        sample = check(args.url.rstrip('/'), path, args.serial)
        probes.append(sample)
        if sample.get('identityMismatch'):
            break
        if len(probes) < 5:
            time.sleep(1.5)  # Normal operator polling cadence, no concurrency or POST retry.
    states = [p for p in probes if p['path'] == '/api/state']
    result = {'schema': 'QC-HTTP-CHECK-1', 'utc': probes[0]['utc'], 'expectedSerial': args.serial,
              'probes': probes, 'stateSuccess': sum(bool(p['ok']) for p in states),
              'stateFailures': sum(not p['ok'] for p in states),
              'stateSlowOver2s': sum(p['rttMs'] > 2000 for p in states),
              'stateMaxMs': max((p['rttMs'] for p in states), default=None)}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    with args.out.open('a', encoding='utf-8') as output:
        output.write(json.dumps(result) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
