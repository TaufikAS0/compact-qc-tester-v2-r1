"""Reject accidental Arduino defaults for the existing legacy 16MB board."""
import hashlib,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
EXPECTED=[('nvs',1,2,0x9000,0x8000),('otadata',1,0,0x17000,0x2000),('phy_init',1,1,0x19000,0x1000),('factory',0,0,0x20000,0x200000),('ota_0',0,0x10,0x220000,0x200000),('ota_1',0,0x11,0x420000,0x200000),('storage',1,0x82,0x620000,0xf0000)]
def parse(data):
 rows=[]
 for at in range(0,len(data)-31,32):
  magic,kind,sub,offset,size,name,flags=struct.unpack_from('<HBBII16sI',data,at)
  if magic!=0x50aa: break
  if flags: raise ValueError('Unexpected partition flags')
  rows.append((name.split(b'\0')[0].decode(),kind,sub,offset,size))
 return rows
def verify(root):
 binary=root/'build/compact_qc_tester.ino.partitions.bin';data=binary.read_bytes()
 if parse(data)!=EXPECTED: raise ValueError('Partition binary differs from approved legacy layout')
 manifestPath=root/'build/firmware-manifest.json';manifest=json.loads(manifestPath.read_text())
 manifest['partitionsSHA256']=hashlib.sha256(data).hexdigest();manifest['factoryOffset']='0x20000';manifest['otaSlotBytes']=2097152
 manifestPath.write_text(json.dumps(manifest,indent=2)+'\n');return manifest['partitionsSHA256']
if __name__=='__main__':print('PASS: legacy partitions, SHA256 '+verify(ROOT))
