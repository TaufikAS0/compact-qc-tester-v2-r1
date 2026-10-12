const {test} = require('node:test');
const assert = require('node:assert/strict');
const {createMerged} = require('../tools/portal-package.cjs');
function fixtures() {
  const boot = Buffer.alloc(256,0x11); boot[0] = 0xe9;
  const app = Buffer.alloc(1024,0x22); app[0] = 0xe9;
  const parts = Buffer.alloc(0x1000,0xff);
  const rows = [['nvs',0x9000,0x8000,1,2],['otadata',0x17000,0x2000,1,0],['phy_init',0x19000,0x1000,1,1],['factory',0x20000,0x200000,0,0],['ota_0',0x220000,0x200000,0,16],['ota_1',0x420000,0x200000,0,17],['storage',0x620000,0xf0000,1,130]];
  rows.forEach(([name,offset,size,type,subtype],index) => {const i=index*32;parts.writeUInt16LE(0x50aa,i);parts[i+2]=type;parts[i+3]=subtype;parts.writeUInt32LE(offset,i+4);parts.writeUInt32LE(size,i+8);parts.fill(0,i+12,i+28);parts.write(name,i+12);});
  return {boot,app,parts};
}
test('full USB image places boot/partitions/app exactly, with erased otadata/NVS', () => {
  const {boot,app,parts}=fixtures(); const merged=createMerged(boot,parts,app);
  assert.deepEqual(merged.subarray(0x1000,0x1100),boot);
  assert.deepEqual(merged.subarray(0x8000,0x9000),parts);
  assert.deepEqual(merged.subarray(0x20000),app);
  assert.ok(merged.subarray(0x9000,0x19000).every(byte=>byte===0xff));
  assert.equal(merged.length,0x20000+app.length);
});
test('rejects bad magic, oversized images and changed legacy offsets', () => {
  const {boot,app,parts}=fixtures();
  assert.throws(()=>createMerged(Buffer.alloc(256),parts,app),/magic/);
  const huge=Buffer.alloc(0x200000,0xe9);assert.throws(()=>createMerged(boot,parts,huge),/region/);
  parts.writeUInt32LE(0x10000,3*32+4);assert.throws(()=>createMerged(boot,parts,app),/factory/);
});
