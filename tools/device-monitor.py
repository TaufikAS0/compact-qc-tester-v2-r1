"""Read-only, bounded Compact LAN health recorder. Never sends POSTs or QC commands."""
import argparse
import datetime as dt
import json
from pathlib import Path
import time
import urllib.error
import urllib.request


def probe(base, path='/api/health'):
    started = time.monotonic()
    try:
        with urllib.request.urlopen(base + path, timeout=5) as response:
            data = json.load(response)
    except urllib.error.HTTPError as error:
        if error.code != 404:
            raise
        with urllib.request.urlopen(base + '/api/state', timeout=5) as response:
            data = json.load(response)
    # Whitelist: full state includes control tokens; never persist the full response.
    return {
        'serial': data.get('serial') or data.get('hardware', {}).get('serial'),
        **{key: data.get(key) for key in ('version', 'buildUTC', 'uptime', 'freeHeap',
                                        'minFreeHeap', 'resetReason', 'maxLoopMs', 'outputActive')},
        'wifi': {key: data.get('wifi', {}).get(key) for key in
                 ('connected', 'rssiDBm', 'bars', 'channel', 'sleepDisabled', 'disconnects',
                  'gotIPs', 'retryRequests', 'lastDisconnectMs', 'lastGotIPMs', 'lastDisconnectReason',
                  'bssid', 'lastRetryResult')},
        'http': {key: data.get('http', {}).get(key) for key in
                 ('requests', 'rejectedHeaders', 'maxHandleMs', 'writeTimeouts')},
        'wifiHistory': [{key: event.get(key) for key in ('uptimeMs', 'reason', 'rssiDBm')}
                        for event in data.get('wifi', {}).get('disconnectHistory', [])[-8:]],
        'rttMs': round((time.monotonic() - started) * 1000, 1),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--url', required=True)
    parser.add_argument('--serial', required=True, help='Refuse monitoring a replacement unit')
    parser.add_argument('--hours', type=float, default=12)
    parser.add_argument('--interval', type=float, default=30)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--state-every', type=int, default=0, help='Also GET QC state every N health samples;0 disables')
    args = parser.parse_args()
    if not 0 < args.hours <= 24 or not 5 <= args.interval <= 3600:
        parser.error('hours must be (0,24], interval5..3600 seconds')
    if not 0 <= args.state_every <= 3600:
        parser.error('state-every must be0..3600')
    if not args.url.startswith(('http://', 'https://')):
        parser.error('Explicit http(s) URL required')
    args.out.mkdir(parents=True, exist_ok=True)
    summary = {'url': args.url, 'serial': args.serial, 'startedUTC': dt.datetime.now(dt.timezone.utc).isoformat(),
               'hoursRequested': args.hours, 'intervalSeconds': args.interval, 'samples': 0, 'success': 0,
               'failures': 0, 'consecutiveFailures': 0, 'maxConsecutiveFailures': 0, 'rebootsObserved': 0,
               'rssiMin': None, 'rssiMax': None, 'rttMaxMs': 0, 'running': True,
               'stateSuccess': 0, 'stateFailures': 0, 'stateMaxMs': 0, 'stateSlowOver2s': 0}
    prior_uptime = None
    until = time.monotonic() + args.hours * 3600
    while time.monotonic() < until:
        stamp = dt.datetime.now(dt.timezone.utc).isoformat()
        sample = {'utc': stamp}
        wrong_unit = False
        try:
            sample.update(probe(args.url.rstrip('/')))
            if sample['serial'] != args.serial:
                wrong_unit = True
                raise RuntimeError('Unit identity changed; monitor stopped')
            sample['ok'] = True
            summary['success'] += 1
            summary['consecutiveFailures'] = 0
            uptime = sample['uptime']
            if prior_uptime is not None and uptime is not None and uptime < prior_uptime:
                summary['rebootsObserved'] += 1  # Includes millis rollover after49d; inspect timestamps.
            prior_uptime = uptime
            rssi = sample['wifi']['rssiDBm']
            if isinstance(rssi, (int, float)):
                summary['rssiMin'] = min(rssi, summary['rssiMin'] if summary['rssiMin'] is not None else rssi)
                summary['rssiMax'] = max(rssi, summary['rssiMax'] if summary['rssiMax'] is not None else rssi)
            summary['rttMaxMs'] = max(summary['rttMaxMs'], sample['rttMs'])
            if args.state_every and summary['samples'] % args.state_every == 0:
                try:
                    state = probe(args.url.rstrip('/'), '/api/state')
                    if state['serial'] != args.serial:
                        wrong_unit = True
                        raise RuntimeError('State unit identity changed; monitor stopped')
                    sample['qcState'] = state
                    summary['stateSuccess'] += 1
                    summary['stateMaxMs'] = max(summary['stateMaxMs'], state['rttMs'])
                    summary['stateSlowOver2s'] += state['rttMs'] > 2000
                except Exception as error:
                    sample['qcStateError'] = str(error)[:240]
                    summary['stateFailures'] += 1
        except Exception as error:
            sample.update(ok=False, error=str(error)[:240])
            summary['failures'] += 1
            summary['consecutiveFailures'] += 1
            summary['maxConsecutiveFailures'] = max(summary['maxConsecutiveFailures'], summary['consecutiveFailures'])
        summary['samples'] += 1
        summary['last'] = sample
        with (args.out / 'samples.jsonl').open('a', encoding='utf-8') as output:
            output.write(json.dumps(sample, ensure_ascii=True) + '\n')
        summary['running'] = not wrong_unit and time.monotonic() + args.interval < until
        temp = args.out / 'summary.tmp'
        temp.write_text(json.dumps(summary, indent=2), encoding='utf-8')
        temp.replace(args.out / 'summary.json')
        if wrong_unit:
            break
        time.sleep(min(args.interval, max(0, until - time.monotonic())))
    summary['running'] = False
    summary['finishedUTC'] = dt.datetime.now(dt.timezone.utc).isoformat()
    (args.out / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')


if __name__ == '__main__':
    main()
