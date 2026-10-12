const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const {execFileSync} = require('node:child_process');
const expected = {
  nvs: [0x9000, 0x8000], otadata: [0x17000, 0x2000], phy_init: [0x19000, 0x1000],
  factory: [0x20000, 0x200000], ota_0: [0x220000, 0x200000],
  ota_1: [0x420000, 0x200000], storage: [0x620000, 0xf0000]
};
function createMerged(boot, partitions, app) {
  if (boot[0] !== 0xe9 || app[0] !== 0xe9) throw Error('Invalid ESP image magic');
  if (boot.length > 0x7000 || partitions.length > 0x1000 || app.length >= 0x200000) throw Error('Image exceeds its region');
  const rows = new Map();
  for (let i = 0; i + 32 <= partitions.length && partitions.readUInt16LE(i) === 0x50aa; i += 32) {
    const name = partitions.subarray(i + 12, i + 28).toString().replace(/\0.*$/, '');
    if (rows.has(name)) throw Error('Duplicate partition');
    rows.set(name, {offset: partitions.readUInt32LE(i + 4), size: partitions.readUInt32LE(i + 8), type: partitions[i + 2], subtype: partitions[i + 3]});
  }
  if (rows.size !== Object.keys(expected).length) throw Error('Unexpected partition count');
  for (const [name, [offset, size]] of Object.entries(expected)) {
    const row = rows.get(name);
    if (!row || row.offset !== offset || row.size !== size) throw Error(`Partition mismatch: ${name}`);
  }
  if (rows.get('factory').type !== 0 || rows.get('factory').subtype !== 0 || rows.get('otadata').type !== 1 || rows.get('otadata').subtype !== 0) throw Error('Invalid boot partition types');
  const merged = Buffer.alloc(0x20000 + app.length, 0xff);
  boot.copy(merged, 0x1000);
  partitions.copy(merged, 0x8000);
  app.copy(merged, 0x20000);
  // otadata stays erased (FF), selecting the factory application. Do NOT
  // include Arduino's default boot_app0 at 0xe000, inside this layout's NVS.
  // This FULL image resets NVS even in fast mode. Full erase also clears storage.
  return merged;
}
function packageBuild(root) {
  if (execFileSync('git', ['status', '--porcelain'], {cwd:root,encoding:'utf8',windowsHide:true}).trim()) throw Error('Commit source before release packaging');
  if (fs.existsSync(path.join(root, 'compact_qc_tester/ota_secrets.h'))) throw Error('Private OTA override is not distributable');
  const config = fs.readFileSync(path.join(root, 'compact_qc_tester/config.h'), 'utf8');
  const version = config.match(/FW_VERSION\[\]\s*=\s*"([^"]+)"/)[1];
  const sourceCommit = execFileSync('git', ['rev-parse', 'HEAD'], {cwd:root,encoding:'utf8',windowsHide:true}).trim();
  const build = path.join(root, 'build');
  const app = fs.readFileSync(path.join(build, 'compact_qc_tester.ino.bin'));
  if (app.readUInt16LE(12) !== 0 || app[3] >> 4 !== 4 || !app.includes(Buffer.from(version))) throw Error('Wrong chip/flash/version');
  const merged = createMerged(fs.readFileSync(path.join(build, 'compact_qc_tester.ino.bootloader.bin')), fs.readFileSync(path.join(build, 'compact_qc_tester.ino.partitions.bin')), app);
  const artifacts = [];
  for (const [type, bytes, offset] of [['full',merged,0],['app-only',app,0x20000]]) {
    const fileName = `TMC_ESP32_16M_v${version}_${type === 'full' ? 'merged' : 'app'}.bin`;
    fs.writeFileSync(path.join(build, fileName), bytes);
    artifacts.push({imageType:type,fileName,offset,sizeBytes:bytes.length,sha256:crypto.createHash('sha256').update(bytes).digest('hex')});
  }
  const report = {version,sourceCommit,buildId:sourceCommit.slice(0,7),flashMode:['qio','qout','dio','dout'][app[2]],createdAt:new Date().toISOString(),artifacts};
  fs.writeFileSync(path.join(build, 'portal-artifacts.json'), JSON.stringify(report,null,2));
  console.log(JSON.stringify(report,null,2));
}
module.exports = {createMerged};
if (require.main === module) packageBuild(path.resolve(__dirname,'..'));
