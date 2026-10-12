"""Generate one UTC identity before compilation; verify that exact identity in the app."""
import argparse, datetime, hashlib, json, re
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
def version(root):
    return re.search(r'FW_VERSION\[\]\s*=\s*"(\d+\.\d+\.\d+)"', (root/'compact_qc_tester/firmware_version.h').read_text())[1]
def generate(root):
    v=version(root)
    utc=datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')
    marker=f'QC_BUILD={v}|{utc}'
    (root/'compact_qc_tester/build_metadata.h').write_text(f'#pragma once\nconstexpr char FW_BUILD_UTC[]="{utc}";\nconstexpr char FW_BUILD_VERSION[]="{v}";\nconstexpr char FW_BUILD_ID[]="{marker}";\n')
    out=root/'build'; out.mkdir(exist_ok=True)
    (out/'build-start.json').write_text(json.dumps(dict(version=v,buildUTC=utc,identity=marker)))
    return marker
def verify(root,binary):
    start=json.loads((root/'build/build-start.json').read_text())
    data=binary.read_bytes()
    if start['version']!=version(root) or start['identity'].encode()+b'\0' not in data:
        raise ValueError('Binary metadata missing/stale or version mismatch')
    if len(data)>=2097152 or len(data)<1024 or data[0]!=0xe9:
        raise ValueError('Invalid ESP32 application/2MB slot limit')
    # ESP image header chip id: classic ESP32 is 0.
    if int.from_bytes(data[12:14],'little')!=0: raise ValueError('Wrong target chip')
    manifest={**start,'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data),'app':binary.name,'fqbn':'esp32:esp32:esp32:FlashSize=16M','core':'3.3.10'}
    (root/'build/firmware-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    return manifest
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('action',choices=['generate','verify']);args=parser.parse_args()
    if args.action=='generate': print(generate(ROOT))
    else: print(json.dumps(verify(ROOT,ROOT/'build/compact_qc_tester.ino.bin'),indent=2))
