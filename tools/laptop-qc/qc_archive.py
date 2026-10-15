"""Append-only local QC snapshots. Import never opens COM or contacts a device."""
import csv
import hashlib
import io
import json
import os
import re
from datetime import datetime, timezone, date
from pathlib import Path

def sanitize(value):
    if isinstance(value, dict): return {k:sanitize(v) for k,v in value.items() if not re.search('token|password|secret|nonce', k, re.I)}
    if isinstance(value, list): return [sanitize(v) for v in value]
    return value

def validate(report):
    if not isinstance(report,dict) or report.get('schema')!='QC-ARCHIVE-1': raise ValueError('Expected QC-ARCHIVE-1 report/bundle')
    s=report.get('snapshot',{});q=s.get('qc',{});serial=s.get('hardware',{}).get('serial','')
    if not re.fullmatch(r'[A-Za-z0-9_-]{1,64}',serial) or not str(q.get('qcCode','')).startswith('QC1-'): raise ValueError('Invalid unit/code')
    if q.get('overall') not in ('PASS','FAIL','INCOMPLETE') or not isinstance(q.get('tests'),list) or len(q['tests'])>80: raise ValueError('Invalid QC status/tests')
    active=[]
    for t in q['tests']:
        if not isinstance(t,dict) or any(not isinstance(t.get(k,''),str) or len(t.get(k,''))>limit for k,limit in [('name',128),('status',64),('detail',4096)]): raise ValueError('Invalid test evidence')
        if t.get('applicable') is not False: active.append(t)
    expected='FAIL' if any(t['status']=='FAIL' for t in active) else 'PASS' if active and all(t['status']=='PASS' for t in active) and q.get('profileSelected',True) else 'INCOMPLETE'
    if q['overall']!=expected: raise ValueError('Overall conflicts with test evidence; import refused')
    if not isinstance(q.get('operator',''),str) or len(q.get('operator',''))>64: raise ValueError('Invalid operator')
    if s.get('qcDate'): date.fromisoformat(s['qcDate'])
    clean=sanitize(report)
    # Imported CSV is rebuilt on export; never execute or trust arbitrary CSV formulas.
    clean.pop('csv',None)
    return clean

class Archive:
    def __init__(self,root): self.root=Path(root)
    def import_reports(self,payload):
        reports=payload.get('reports') if isinstance(payload,dict) and payload.get('schema')=='QC-ARCHIVE-BUNDLE-1' else [payload]
        if not isinstance(reports,list) or not 1<=len(reports)<=50: raise ValueError('Import1..50 snapshots per bundle')
        clean=[validate(r) for r in reports] # validate entire batch before writing
        self.root.mkdir(parents=True,exist_ok=True);added=duplicates=0
        for r in clean:
            data=json.dumps(r,sort_keys=True,ensure_ascii=False,separators=(',',':'));identity=hashlib.sha256(data.encode()).hexdigest()
            path=self.root/(identity+'.json')
            entry={'id':identity,'importedUTC':datetime.now(timezone.utc).isoformat(),'report':r}
            try:
                with path.open('x',encoding='utf-8') as f:
                    try: json.dump(entry,f,ensure_ascii=False,indent=2);f.flush();os.fsync(f.fileno())
                    except BaseException:
                        f.close();path.unlink(missing_ok=True);raise
                added+=1
            except FileExistsError: duplicates+=1
        return {'added':added,'duplicates':duplicates}
    def read(self,identity):
        if not re.fullmatch('[a-f0-9]{64}',identity): raise ValueError('Invalid archive ID')
        with (self.root/(identity+'.json')).open(encoding='utf-8') as f: entry=json.load(f)
        clean=validate(entry['report']);digest=hashlib.sha256(json.dumps(clean,sort_keys=True,ensure_ascii=False,separators=(',',':')).encode()).hexdigest()
        if digest!=identity or entry.get('id')!=identity: raise ValueError('Archive checksum mismatch')
        return entry
    def list(self):
        items=[];errors=[]
        for p in sorted(self.root.glob('*.json')):
            try:
                e=self.read(p.stem);s=e['report']['snapshot'];q=s['qc']
                items.append({'id':e['id'],'serial':s['hardware']['serial'],'profile':q['variant'],'status':q['overall'],'qcCode':q['qcCode'],'operator':q.get('operator',''),'qcDate':s.get('qcDate',''),'capturedAt':e['report'].get('capturedAtLaptop',''),'importedUTC':e['importedUTC'],'failures':[t for t in q['tests'] if t.get('applicable') is not False and t['status']=='FAIL']})
            except (ValueError,KeyError,TypeError,OSError) as exc: errors.append({'file':p.name,'error':str(exc)})
        return {'items':sorted(items,key=lambda p:p['importedUTC'],reverse=True),'errors':errors}

def report_csv(entry):
    s=entry['report']['snapshot'];q=s['qc'];h=s['hardware']
    values={'serialNumber':h['serial'],'productName':'Compact QC Tester','productType':q['variant'],'productionDate':s.get('productionDate',''),'qcStatus':q['overall'],'qcDate':s.get('qcDate',''),'qcPic':q.get('operator',''),'status':'','deliveredAt':'','deliveredTo':'','qcCode':q['qcCode'],'firmwareVersion':s.get('version',''),'buildUTC':s.get('buildUTC',''),'sessionId':q.get('sessionId',''),'profileRevision':q.get('profileRevision',''),'recipeRevision':q.get('recipe',{}).get('revision',''),'recipeCRC':q.get('recipe',{}).get('crc','')}
    for t in q['tests']:
        for k in ('status','detail','applicable'): values[t['name']+'.'+k]=t.get(k,'')
    def safe(v):
        text=str(v)
        return "'"+text if text.lstrip().startswith(('=','+','-','@')) else text
    f=io.StringIO(newline='');w=csv.writer(f);w.writerow(values);w.writerow([safe(v) for v in values.values()]);return ('\ufeff'+f.getvalue()).encode()
