const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../compact_qc_tester/web_ui.h'), 'utf8');
const script = source.match(/<script>([\s\S]*?)<\/script>/)[1];
const elements = new Map();
const element = id => {
  if (!elements.has(id)) elements.set(id, {value: '', textContent: '', options: [], disabled: false, add(o) {this.options.push(o);}});
  return elements.get(id);
};
let downloadBlob, clicked = false;
const state = {board:'V2_R1',version:'0.1.0',chip:'ESP32',ssid:'COMPACT-QC',token:'session',uptime:0,relayEnabled:false,ledEnabled:false,inputs:[{name:'DI1',pin:13,value:0}],leds:['LED2']};
const context = vm.createContext({
  document:{getElementById:element,createElement:()=>({click(){clicked=true;}})},
  fetch:async url=>({ok:true,json:async()=>url==='/api/state'?state:{ok:true}}),
  URLSearchParams, Blob, Date, JSON, Object, Error, String, Option:function(text,value){this.text=text;this.value=value;},
  URL:{createObjectURL(blob){downloadBlob=blob;return 'blob:test';},revokeObjectURL(){}},
  setTimeout:()=>0,
});
vm.runInContext(script, context); // Syntax check plus initial connection.
(async()=>{
  await new Promise(resolve=>setImmediate(resolve));
  assert.equal(element('relayOn').disabled,true,'unverified relays must remain locked');
  assert.equal(element('ledOn').disabled,true,'unverified LEDs must remain locked');
  element('led').value='0';
  element('ledPass').onclick();
  assert.match(element('error').textContent,/Jalankan tes/,'operator cannot PASS an unattempted output');
  element('download').onclick();
  assert.equal(clicked,false,'report requires operator and unit identity');
  element('unit').value='QC-001';element('operator').value='Operator';
  element('download').onclick();
  let report=JSON.parse(await downloadBlob.text());
  assert.equal(report.overall,'INCOMPLETE');
  assert.equal(report.tests.SD.status,'BELUM DIUJI');
  assert.equal(report.tests['Relay:0'].status,'BELUM DIUJI');
  assert.ok(report.functionalTestsNotImplemented.includes('Watchdog'));
  vm.runInContext("verdicts.SD={status:'FAIL',detail:'mount failed'}",context);
  element('download').onclick();report=JSON.parse(await downloadBlob.text());
  assert.equal(report.overall,'FAIL','a failed test must appear in overall result');
  vm.runInContext("verdicts.SD={status:'PASS'}; attempted.add('LED:0')",context);
  element('ledPass').onclick();element('download').onclick();
  report=JSON.parse(await downloadBlob.text());
  assert.equal(report.tests['LED:0'].status,'PASS');
  assert.equal(report.overall,'INCOMPLETE','partial evidence cannot pass the entire board');
  await element('new').onclick();element('download').onclick();report=JSON.parse(await downloadBlob.text());
  assert.equal(report.tests['LED:0'].status,'BELUM DIUJI','new inspection clears prior unit results');
  console.log('PASS: output locks, confirmation guard, identity, missing tests, fail propagation, partial results, session reset');
})().catch(e=>{console.error(e);process.exitCode=1;});
