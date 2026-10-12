// Visual preview ONLY: all hardware actions are rejected; no fictitious QC PASS.
const http=require('node:http'),fs=require('node:fs'),path=require('node:path');
const root=path.join(__dirname,'..');
const names=['DI1','DI2','DI3','DI4','DMM mode','Normal mode','Webserver mode'];
const leds=['LED2 WiFi','LED3 Critical','LED4 Radio','LED11 Config DI1','LED12 Config DI2','LED13 Config DI3','LED14 Config DI4','LED15 Config DO1','LED16 Config DO2','LED17 Serial RX','LED18 Serial TX'];
const state={board:'TMC TOP V2_R1 / BOTTOM V3_R0',version:'1.0.0',buildUTC:'PREVIEW • belum terhubung',token:'preview',staConnected:false,apIP:'PREVIEW',tplReady:false,tplError:'Preview only',relayEnabled:false,ledEnabled:false,inputs:names.map((name,i)=>({name,pin:[13,14,25,26,2,'GPIO2 LOW & GPIO39 LOW',39][i],valid:false})),leds,hardware:{serial:'TMC-PREVIEW',oledReady:false,adcValid:false,identityValid:false,identityDetail:'EEPROM belum dibaca; preview',calibration:{pending:-1,points:Array.from({length:4},()=>({captured:false})),detail:'Preview only'},rtc:{valid:false,detail:'Preview only'}},qc:{overall:'INCOMPLETE',inputs:names.map(()=>({cycles:0})),outputs:Array.from({length:13},()=>({commandedOn:false})),tests:[...['OLED','I2C scan','RTC','EEPROM','AHT10','ADS / 4-20mA','STUSB4500','Ethernet','SD'],...names.map(n=>'Input:'+n),...leds.map((_,i)=>'LED:'+i),'Relay:0','Relay:1','RS485','RS232'].map(name=>({name,status:name.startsWith('RS')?'DITUNDA':'BELUM DIUJI',detail:'PREVIEW • tidak terhubung hardware'}))}};
http.createServer((req,res)=>{
 if(req.url==='/'){const source=fs.readFileSync(path.join(root,'compact_qc_tester/web_ui.h'),'utf8');const html=source.match(/R"HTML\(([\s\S]*)\)HTML"/)[1].replace('<main>','<main><div class="notice">PREVIEW — tidak terhubung hardware. Tombol tidak melakukan QC.</div>');res.writeHead(200,{'Content-Type':'text/html; charset=utf-8'});res.end(html);return;}
 res.setHeader('Content-Type','application/json');
 if(req.url==='/api/state'){res.end(JSON.stringify(state));return;}
 if(['/api/heartbeat','/api/stop'].includes(req.url)){res.end('{"ok":true}');return;}
 res.writeHead(501);res.end('{"error":"PREVIEW: tes memerlukan board ESP32."}');
}).listen(8769,'127.0.0.1',()=>console.log('Preview http://127.0.0.1:8769'));
