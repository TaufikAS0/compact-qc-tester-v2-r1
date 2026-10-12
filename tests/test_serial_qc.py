import sys
import threading
import unittest
from unittest.mock import patch
import tempfile
import types
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/laptop-qc'))
import protocol as p
import server

class Fixture:
    def __init__(self, fault=None): self.queue = bytearray(); self.fault = fault
    def write(self, data):
        kind, run, seq, _ = p.decode(data)
        reply = p.encode(p.RESPONSE if kind == p.REQUEST else p.ACK_REPLY, run, seq)
        if self.fault == 'echo': reply = data
        if self.fault == 'crc': reply = reply[:-1] + bytes([reply[-1] ^ 1])
        if self.fault == 'sequence': reply = p.encode(p.RESPONSE, run, seq + 1)
        if self.fault != 'timeout': self.queue.extend(reply)
        return len(data)
    def read(self, size):
        out = bytes(self.queue[:min(size, 3)]); del self.queue[:len(out)]
        return out

class ProtocolTests(unittest.TestCase):
    def test_crc_known(self):
        self.assertEqual(p.binascii.crc_hqx(b'123456789', 0xffff), 0x29b1)
    def test_all_payloads_and_fragmentation(self):
        progress = []
        times = p.exercise(Fixture(), 0x12345678, 100, .1, threading.Event(), lambda n, _: progress.append(n))
        self.assertEqual(progress[-1], 100); self.assertEqual(len(times), 100)
    def test_echo_crc_sequence_timeout_never_pass(self):
        for fault in ['echo', 'crc', 'sequence', 'timeout']:
            with self.subTest(fault=fault), self.assertRaises((ValueError, TimeoutError)):
                p.exercise(Fixture(fault), 4, 10, .01, threading.Event(), lambda *_: None)
    def test_cancel(self):
        e = threading.Event(); e.set()
        with self.assertRaises(InterruptedError): p.exercise(Fixture(), 1, 10, .1, e, lambda *_: None)
    def test_bad_length_and_stale_run(self):
        data = p.encode(p.REQUEST, 1, 0)
        with self.assertRaises(ValueError): p.decode(data[:-1])
        self.assertNotEqual(data, p.encode(p.REQUEST, 2, 0))
    def test_settings(self):
        cfg = dict(device='http://192.168.4.1', port='COM99')
        self.assertEqual(server.settings_from(cfg)['baud'], 9600)
        for key, value in [('device','http://127.0.0.1'), ('device','http://example.com'),
                           ('packets',0), ('baud',123), ('timeoutMs',1)]:
            with self.subTest(key=key), self.assertRaises(ValueError): server.settings_from({**cfg,key:value})
        with self.assertRaises(ValueError): server.settings_from({**cfg,'baud':1200,'timeoutMs':2000})
    def test_csv_formula_and_bom(self):
        data = server.report_csv({'name':'=TEST','detail':'a,"b"\r\nc'})
        self.assertTrue(data.startswith(b'\xef\xbb\xbf'))
        self.assertIn("'=TEST", data.decode()); self.assertIn('""b""', data.decode())
    def test_idle_no_pass(self):
        s = server.snapshot(); self.assertFalse(s['active']); self.assertEqual(s['status'], 'IDLE')
        self.assertNotIn('localToken', s)
    def test_worker_pass_and_reboot_rejection(self):
        cfg = server.settings_from(dict(device='http://192.168.4.1', port='COM99', packets=10,mode='RS485'))
        before = dict(token='T', hardware={'serial':'TMC-TEST'}, version='1.2.0', buildUTC='TEST',
                      qc={'profileSelected':True,'variant':'M1','sessionId':'S'})
        class Port(Fixture):
            def __enter__(self): return self
            def __exit__(self, *_): pass
            def reset_input_buffer(self): pass
            def reset_output_buffer(self): pass
        for reboot in (False,True):
            calls = []
            final = dict(token='NEW' if reboot else 'T',qc={'sessionId':'S','qcCode':'TEST',
                'serial':{'active':False,'status':'PASS','completed':10,'runId':'12345678'}})
            def request(base,path,args=None):
                calls.append((path,args))
                if path.endswith('/start'): return {'runId':'12345678'}
                if path.endswith('/stop'): return {'ok':True}
                return before if len(calls)==1 else final
            with tempfile.TemporaryDirectory() as tmp, patch.object(server,'ROOT',Path(tmp)), \
                 patch.object(server,'request',request), patch.dict(sys.modules,{'serial':types.SimpleNamespace(Serial=lambda *_a,**_kw:Port())}):
                server.CANCEL.clear(); server.run_test(cfg)
                report = server.snapshot()['report']
                self.assertEqual(report['status'],'FAIL' if reboot else 'PASS')
                self.assertNotIn('token',report)
                self.assertEqual(calls[1][1]['mode'],'RS485')
                self.assertEqual(any(p.endswith('/stop') for p,_ in calls),reboot)

if __name__ == '__main__': unittest.main()
