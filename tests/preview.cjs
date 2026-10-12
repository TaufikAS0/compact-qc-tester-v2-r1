// Local UI preview only. Does not connect to hardware or simulate QC success.
const http = require('node:http');
const fs = require('node:fs');
const path = require('node:path');
const source = fs.readFileSync(path.join(__dirname, '../compact_qc_tester/web_ui.h'), 'utf8');
const html = source.match(/R"HTML\(([\s\S]*)\)HTML"/)[1].replace('<main>', '<main><p style="background:#ffe7ab;padding:12px"><strong>PREVIEW — tidak terhubung hardware.</strong> Tombol tes tidak melakukan pengujian.</p>');
http.createServer((req,res)=>{
  if(req.url==='/'){res.writeHead(200,{'Content-Type':'text/html; charset=utf-8'});res.end(html);return;}
  res.setHeader('Content-Type','application/json');
  if(req.url==='/api/state'){res.end(JSON.stringify({board:'TMC PREVIEW',version:'0.5.0',chip:'Not connected',ssid:'DEMO',token:'preview',uptime:0,staIP:'Tidak terhubung',apIP:'Tidak terhubung',relayEnabled:false,ledEnabled:false,tplReady:false,tplError:'Preview only',hardware:{serial:'TMC-PREVIEW',hwTop:'TMC_T_V2_R1',hwBottom:'TMC_B_V3_R0',adcValid:false},qc:{operator:'',active:'',overall:'INCOMPLETE',tests:['OLED','EEPROM','Ethernet','RS485','RS232','ADS / 4-20mA'].map(name=>({name,status:name.startsWith('RS')?'DITUNDA':'BELUM DIUJI',detail:'Preview only'}))},inputs:[{name:'DI1',pin:13,valid:true,value:0},{name:'Display button',pin:'MCP1B3',valid:false}],leds:['LED2 WiFi','LED3 Critical']}));return;}
  if(req.url==='/api/heartbeat'||req.url==='/api/stop'){res.end('{"ok":true}');return;}
  res.writeHead(501);res.end('{"error":"PREVIEW: tes memerlukan board ESP32."}');
}).listen(8769,'127.0.0.1',()=>console.log('UI preview: http://127.0.0.1:8769'));
