import importlib.util,json,tempfile,unittest
from pathlib import Path
spec=importlib.util.spec_from_file_location('metadata',Path(__file__).resolve().parents[1]/'scripts/firmware_metadata.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class MetadataTest(unittest.TestCase):
 def test_missing_stale_and_matching(self):
  with tempfile.TemporaryDirectory() as tmp:
   root=Path(tmp);(root/'compact_qc_tester').mkdir();v=root/'compact_qc_tester/firmware_version.h';v.write_text('constexpr char FW_VERSION[]="1.0.0";')
   marker=m.generate(root);self.assertIn('QC_BUILD=1.0.0|',marker);self.assertIn('Z',marker)
   binary=root/'build/app.bin';data=bytearray(1024);data[0]=0xe9;binary.write_bytes(data)
   with self.assertRaises(ValueError):m.verify(root,binary)
   binary.write_bytes(data+marker.encode()+b'\0');result=m.verify(root,binary);self.assertEqual(result['version'],'1.0.0');self.assertEqual(len(result['sha256']),64)
   v.write_text('constexpr char FW_VERSION[]="1.0.1";')
   with self.assertRaises(ValueError):m.verify(root,binary)
   m.generate(root)
   with self.assertRaises(ValueError):m.verify(root,binary)
if __name__=='__main__':unittest.main()
