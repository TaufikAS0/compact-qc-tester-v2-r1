import csv
import importlib.util
import io
import json
from pathlib import Path
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('qc_archive',ROOT/'tools/laptop-qc/qc_archive.py')
archive=importlib.util.module_from_spec(spec);spec.loader.exec_module(archive)
def report(status='FAIL',session='A',serial='TMC123'):
    return {'schema':'QC-ARCHIVE-1','capturedAtLaptop':'2026-10-15T00:00:00Z','csv':'UNTRUSTED FORMULA', 'snapshot':{'version':'1.9.0','qcDate':'2026-10-15','hardware':{'serial':serial,'secret':'PRIVATE'},'qc':{'variant':'M1','sessionId':session,'operator':'=formula','overall':status,'qcCode':'QC1-M1-'+session,'tests':[{'name':'RTC','status':status,'detail':'=danger','applicable':True}]},'token':'PRIVATE'}}
class ArchiveTests(unittest.TestCase):
    def test_append_dedup_and_retest_without_overwrite(self):
        with tempfile.TemporaryDirectory() as folder:
            a=archive.Archive(folder);first=report();self.assertEqual(a.import_reports(first)['added'],1)
            original={p.name:p.read_bytes() for p in Path(folder).glob('*.json')}
            self.assertEqual(a.import_reports(first)['duplicates'],1)
            self.assertEqual(a.import_reports(report('PASS','B'))['added'],1)
            for name,content in original.items():self.assertEqual((Path(folder)/name).read_bytes(),content)
            listing=a.list();self.assertEqual(len(listing['items']),2);self.assertEqual(listing['errors'],[])
            for item in listing['items']:
                entry=a.read(item['id']);self.assertNotIn('PRIVATE',json.dumps(entry));self.assertNotIn('csv',entry['report'])
                rows=list(csv.reader(io.StringIO(archive.report_csv(entry).decode('utf-8-sig'))));self.assertEqual(rows[0][:10],['serialNumber','productName','productType','productionDate','qcStatus','qcDate','qcPic','status','deliveredAt','deliveredTo'])
                self.assertEqual(dict(zip(*rows))['qcPic'],"'=formula")
    def test_bad_batch_writes_nothing_and_no_path_traversal(self):
        with tempfile.TemporaryDirectory() as folder:
            a=archive.Archive(folder);bad=report('PASS');bad['snapshot']['qc']['tests'][0]['status']='FAIL'
            with self.assertRaises(ValueError):a.import_reports({'schema':'QC-ARCHIVE-BUNDLE-1','reports':[report(),bad]})
            self.assertEqual(list(Path(folder).glob('*.json')),[])
            for identity in ['../../file','bad','a'*65]:
                with self.assertRaises(ValueError):a.read(identity)
    def test_corrupt_archive_is_visible_not_silently_lost(self):
        with tempfile.TemporaryDirectory() as folder:
            a=archive.Archive(folder);a.import_reports(report());p=next(Path(folder).glob('*.json'));raw=p.read_text();p.write_text(raw.replace('=formula','changed'))
            listing=a.list();self.assertEqual(len(listing['errors']),1);self.assertTrue(p.exists())
    def test_incomplete_not_promoted_and_bundle_multi_device(self):
        with tempfile.TemporaryDirectory() as folder:
            a=archive.Archive(folder);r=report('INCOMPLETE');r['snapshot']['qc']['tests'][0]['status']='UNVERIFIED'
            a.import_reports({'schema':'QC-ARCHIVE-BUNDLE-1','reports':[r,report('FAIL','B','TMC456')]})
            self.assertEqual({p['serial'] for p in a.list()['items']},{'TMC123','TMC456'})
            self.assertIn('INCOMPLETE',{p['status'] for p in a.list()['items']})
if __name__=='__main__':unittest.main()
