// ==================== WEB OTA UPDATE ====================

const char OTA_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GyverLamp2 Update</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#e0e0e0;padding:16px;max-width:420px;margin:0 auto}
h1{color:#e94560;margin-bottom:4px;font-size:1.4em}
.sub{color:#888;margin-bottom:16px;font-size:.85em}
.card{background:#16213e;border-radius:12px;padding:16px;margin-bottom:12px}
.info{display:flex;justify-content:space-between;margin-bottom:6px;font-size:.9em}
.info .val{color:#e94560}
input[type=file]{display:none}
.upload-label{display:block;width:100%;padding:12px;background:#0f3460;color:#aaa;border:2px dashed #333;border-radius:8px;text-align:center;cursor:pointer;margin-bottom:8px;font-size:.95em}
.upload-label:hover{border-color:#e94560;color:#fff}
.upload-label.has-file{border-color:#4caf50;color:#4caf50}
button{width:100%;padding:12px;background:#e94560;color:#fff;border:none;border-radius:8px;font-size:1em;cursor:pointer}
button:hover{background:#c73650}
button:disabled{background:#555;cursor:wait}
.progress{width:100%;height:24px;background:#0f3460;border-radius:12px;overflow:hidden;margin:12px 0;display:none}
.progress-bar{height:100%;background:linear-gradient(90deg,#e94560,#4caf50);width:0%;transition:width .3s;border-radius:12px;text-align:center;line-height:24px;font-size:.8em;color:#fff}
.ok{color:#4caf50;text-align:center;padding:20px;font-size:1.1em}
.err{color:#e94560;text-align:center;padding:10px;font-size:.9em}
a{color:#e94560;text-decoration:none}
a:hover{text-decoration:underline}
</style></head><body>
<h1>GyverLamp2</h1>
<div class="sub" id="subtitle"></div>
<div class="card">
<div class="info"><span id="lVer"></span><span class="val">v%VER%</span></div>
<div class="info"><span>IP</span><span class="val">%IP%</span></div>
<div class="info"><span id="lHeap"></span><span class="val">%HEAP% bytes</span></div>
<div class="info"><span id="lUp"></span><span class="val">%UPTIME%</span></div>
</div>
<div class="card" id="uploadCard">
<label class="upload-label" id="fileLabel" for="fw"><span id="lSelect"></span></label>
<input type="file" id="fw" accept=".bin" onchange="fileSelected(this)">
<div class="progress" id="prog"><div class="progress-bar" id="progBar">0%</div></div>
<button onclick="upload()" id="uploadBtn" disabled></button>
</div>
<div id="result"></div>
<p style="text-align:center;margin-top:16px;font-size:.8em;color:#555"><a href="/">&larr; <span id="lBack"></span></a></p>
<script>
var L=navigator.language&&navigator.language.startsWith('ru')?'ru':'en';
var T={
subtitle:{ru:'Обновление прошивки',en:'Firmware Update'},
ver:{ru:'Версия',en:'Version'},
heap:{ru:'Свободно',en:'Free RAM'},
up:{ru:'Время работы',en:'Uptime'},
select:{ru:'Выберите файл .bin',en:'Select .bin file'},
upload:{ru:'Загрузить прошивку',en:'Upload Firmware'},
uploading:{ru:'Загрузка...',en:'Uploading...'},
ok:{ru:'Обновление успешно! Лампа перезагружается...',en:'Update successful! Lamp is rebooting...'},
fail:{ru:'Ошибка обновления!',en:'Update failed!'},
back:{ru:'Назад',en:'Back'}
};
function t(k){return T[k][L]||T[k].en}
document.getElementById('subtitle').textContent=t('subtitle');
document.getElementById('lVer').textContent=t('ver');
document.getElementById('lHeap').textContent=t('heap');
document.getElementById('lUp').textContent=t('up');
document.getElementById('lSelect').textContent=t('select');
document.getElementById('uploadBtn').textContent=t('upload');
document.getElementById('lBack').textContent=t('back');
var selFile=null;
function fileSelected(inp){
  if(inp.files.length>0){
    selFile=inp.files[0];
    document.getElementById('fileLabel').classList.add('has-file');
    document.getElementById('fileLabel').querySelector('span').textContent=selFile.name+' ('+Math.round(selFile.size/1024)+' KB)';
    document.getElementById('uploadBtn').disabled=false;
  }
}
function upload(){
  if(!selFile)return;
  var fd=new FormData();
  fd.append('update',selFile);
  var xhr=new XMLHttpRequest();
  xhr.open('POST','/update',true);
  document.getElementById('uploadBtn').disabled=true;
  document.getElementById('uploadBtn').textContent=t('uploading');
  document.getElementById('prog').style.display='block';
  xhr.upload.onprogress=function(e){
    if(e.lengthComputable){
      var p=Math.round(e.loaded/e.total*100);
      document.getElementById('progBar').style.width=p+'%';
      document.getElementById('progBar').textContent=p+'%';
    }
  };
  xhr.onload=function(){
    document.getElementById('uploadCard').style.display='none';
    try{
      var r=JSON.parse(xhr.responseText);
      if(r.ok)document.getElementById('result').innerHTML='<div class="ok">'+t('ok')+'</div>';
      else document.getElementById('result').innerHTML='<div class="err">'+t('fail')+'</div>';
    }catch(e){
      document.getElementById('result').innerHTML='<div class="ok">'+t('ok')+'</div>';
    }
  };
  xhr.onerror=function(){
    document.getElementById('result').innerHTML='<div class="ok">'+t('ok')+'</div>';
  };
  xhr.send(fd);
}
</script></body></html>)rawliteral";

const char MQTT_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GyverLamp2 MQTT</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#e0e0e0;padding:16px;max-width:420px;margin:0 auto}
h1{color:#e94560;margin-bottom:4px;font-size:1.4em}
.sub{color:#888;margin-bottom:16px;font-size:.85em}
.card{background:#16213e;border-radius:12px;padding:16px;margin-bottom:12px}
label{display:block;margin-bottom:4px;font-size:.9em;color:#aaa}
input[type=text],input[type=password],input[type=number]{width:100%;padding:10px;border:1px solid #333;border-radius:8px;background:#0f3460;color:#fff;font-size:1em;margin-bottom:12px}
.toggle{display:flex;align-items:center;gap:10px;margin-bottom:12px}
.toggle input{width:auto;margin:0}
.sw{position:relative;width:48px;height:26px;cursor:pointer}
.sw input{opacity:0;width:0;height:0}
.sl{position:absolute;top:0;left:0;right:0;bottom:0;background:#333;border-radius:13px;transition:.3s}
.sl:before{content:"";position:absolute;height:20px;width:20px;left:3px;bottom:3px;background:#888;border-radius:50%;transition:.3s}
.sw input:checked+.sl{background:#e94560}
.sw input:checked+.sl:before{transform:translateX(22px);background:#fff}
button{width:100%;padding:12px;background:#e94560;color:#fff;border:none;border-radius:8px;font-size:1em;cursor:pointer;margin-top:4px}
button:hover{background:#c73650}
.ok{color:#4caf50;text-align:center;padding:16px;font-size:1em}
.err{color:#e94560;text-align:center;padding:16px;font-size:1em}
a{color:#e94560;text-decoration:none}
a:hover{text-decoration:underline}
.status{text-align:center;padding:8px;border-radius:8px;margin-bottom:12px;font-size:.9em}
.st-on{background:#1b4332;color:#4caf50}
.st-off{background:#3d1f1f;color:#e94560}
.st-dis{background:#1a1a2e;color:#555}
</style></head><body>
<h1>GyverLamp2</h1>
<div class="sub" id="subtitle"></div>
<div class="status %MQTTCLASS%" id="mqttStatus">%MQTTSTATUS%</div>
<form method="POST" action="/mqtt">
<div class="card">
<div class="toggle">
<label class="sw"><input type="checkbox" name="enabled" value="1" %MQTTEN%><span class="sl"></span></label>
<span id="lEnable"></span>
</div>
<label id="lHost"></label>
<input type="text" name="host" value="%MQTTHOST%" maxlength="31">
<label id="lPort"></label>
<input type="number" name="port" value="%MQTTPORT%" min="1" max="65535">
<label id="lLogin"></label>
<input type="text" name="login" value="%MQTTLOGIN%" maxlength="15">
<label id="lPass"></label>
<input type="password" name="pass" placeholder="********" maxlength="15">
<div style="font-size:.75em;color:#555;margin:-8px 0 12px" id="lPassHint"></div>
<button type="submit" id="saveBtn"></button>
</div>
</form>
<div id="result"></div>
<div class="card" style="margin-top:12px">
<div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px">
<span style="font-size:.9em;color:#aaa" id="lLog"></span>
<span style="font-size:.75em;color:#555" id="logAge"></span>
</div>
<pre id="logBox" style="background:#0a0e1a;border-radius:8px;padding:10px;font-size:.78em;color:#8f8;min-height:60px;max-height:200px;overflow-y:auto;white-space:pre-wrap;word-break:break-all;font-family:monospace"></pre>
</div>
<p style="text-align:center;margin-top:16px;font-size:.8em;color:#555"><a href="/">&larr; <span id="lBack"></span></a></p>
<script>
var L=navigator.language&&navigator.language.startsWith('ru')?'ru':'en';
var T={
subtitle:{ru:'MQTT / Home Assistant',en:'MQTT / Home Assistant'},
log:{ru:'MQTT Лог',en:'MQTT Log'},
enable:{ru:'MQTT включён',en:'MQTT Enabled'},
host:{ru:'Сервер (IP или hostname)',en:'Broker (IP or hostname)'},
port:{ru:'Порт',en:'Port'},
login:{ru:'Логин',en:'Username'},
pass:{ru:'Пароль',en:'Password'},
passHint:{ru:'Оставьте пустым чтобы не менять',en:'Leave empty to keep current'},
save:{ru:'Сохранить',en:'Save'},
back:{ru:'Назад',en:'Back'}
};
function t(k){return T[k][L]||T[k].en}
document.getElementById('subtitle').textContent=t('subtitle');
document.getElementById('lEnable').textContent=t('enable');
document.getElementById('lHost').textContent=t('host');
document.getElementById('lPort').textContent=t('port');
document.getElementById('lLogin').textContent=t('login');
document.getElementById('lPass').textContent=t('pass');
document.getElementById('lPassHint').textContent=t('passHint');
document.getElementById('saveBtn').textContent=t('save');
document.getElementById('lBack').textContent=t('back');
document.getElementById('lLog').textContent=t('log');
function fetchLog(){
fetch('/mqtt/log').then(function(r){return r.text()}).then(function(d){
var b=document.getElementById('logBox');
b.textContent=d||'(empty)';
b.scrollTop=b.scrollHeight;
document.getElementById('logAge').textContent=new Date().toLocaleTimeString();
}).catch(function(){});}
fetchLog();setInterval(fetchLog,3000);
</script></body></html>)rawliteral";

const char STATUS_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GyverLamp2</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#e0e0e0;padding:16px;max-width:420px;margin:0 auto}
h1{color:#e94560;margin-bottom:4px;font-size:1.4em}
.sub{color:#888;margin-bottom:16px;font-size:.85em}
.card{background:#16213e;border-radius:12px;padding:16px;margin-bottom:12px}
.info{display:flex;justify-content:space-between;margin-bottom:6px;font-size:.9em}
.info .val{color:#e94560}
a.btn{display:block;width:100%;padding:12px;background:#e94560;color:#fff;border:none;border-radius:8px;font-size:1em;cursor:pointer;text-align:center;text-decoration:none;margin-top:8px}
a.btn:hover{background:#c73650}
</style></head><body>
<h1>GyverLamp2 %ID%</h1>
<div class="sub">%STATE%</div>
<div class="card">
<div class="info"><span>Firmware</span><span class="val">v%VER%</span></div>
<div class="info"><span>IP</span><span class="val">%IP%</span></div>
<div class="info"><span>WiFi</span><span class="val">%SSID%</span></div>
<div class="info"><span>RSSI</span><span class="val">%RSSI% dBm</span></div>
<div class="info"><span>Free RAM</span><span class="val">%HEAP% bytes</span></div>
<div class="info"><span>Uptime</span><span class="val">%UPTIME%</span></div>
<div class="info"><span>MQTT</span><span class="val">%MQTT%</span></div>
</div>
<a class="btn" href="/mqtt">MQTT / Home Assistant</a>
<a class="btn" href="/update" style="background:#0f3460;margin-top:6px">Firmware Update</a>
</body></html>)rawliteral";

bool checkWebAuth() {
  if (!webServer.authenticate(cfg.webUser, cfg.webPass)) {
    webServer.requestAuthentication();
    return false;
  }
  return true;
}

String getUptime() {
  uint32_t sec = millis() / 1000;
  uint32_t m = sec / 60;
  uint32_t h = m / 60;
  uint32_t d = h / 24;
  char buf[20];
  snprintf(buf, sizeof(buf), "%lud %luh %lum", d, h % 24, m % 60);
  return String(buf);
}

String templateReplace(const char* html) {
  String page = FPSTR(html);
  page.replace("%VER%", String(GL_VERSION));
  page.replace("%IP%", WiFi.localIP().toString());
  page.replace("%HEAP%", String(ESP.getFreeHeap()));
  page.replace("%UPTIME%", getUptime());
  page.replace("%ID%", deviceId);
  page.replace("%SSID%", String(cfg.ssid));
  page.replace("%RSSI%", String(WiFi.RSSI()));
  page.replace("%STATE%", cfg.state ? "ON" : "OFF");
  page.replace("%MQTT%", cfg.mqtt ? (mqttClient.connected() ? "Connected" : "Disconnected") : "Disabled");
  return page;
}

void handleStatusPage() {
  if (!checkWebAuth()) return;
  webServer.send(200, "text/html", templateReplace(STATUS_HTML));
}

void handleOTAPage() {
  if (!checkWebAuth()) return;
  webServer.send(200, "text/html", templateReplace(OTA_HTML));
}

void handleMqttPage() {
  if (!checkWebAuth()) return;
  String page = FPSTR(MQTT_HTML);
  page.replace("%MQTTHOST%", String(cfg.mqttHost));
  page.replace("%MQTTPORT%", String(cfg.mqttPort > 0 ? cfg.mqttPort : 1883));
  page.replace("%MQTTLOGIN%", String(cfg.mqttLogin));
  page.replace("%MQTTEN%", cfg.mqtt ? "checked" : "");
  if (cfg.mqtt) {
    if (mqttClient.connected()) {
      page.replace("%MQTTCLASS%", "st-on");
      page.replace("%MQTTSTATUS%", "MQTT Connected");
    } else {
      page.replace("%MQTTCLASS%", "st-off");
      page.replace("%MQTTSTATUS%", "MQTT Disconnected");
    }
  } else {
    page.replace("%MQTTCLASS%", "st-dis");
    page.replace("%MQTTSTATUS%", "MQTT Disabled");
  }
  webServer.send(200, "text/html", page);
}

void handleMqttSave() {
  if (!checkWebAuth()) return;

  cfg.mqtt = webServer.hasArg("enabled") ? 1 : 0;

  if (webServer.hasArg("host")) {
    strncpy(cfg.mqttHost, webServer.arg("host").c_str(), 31);
    cfg.mqttHost[31] = 0;
  }
  if (webServer.hasArg("port")) {
    cfg.mqttPort = webServer.arg("port").toInt();
  }
  if (webServer.hasArg("login")) {
    strncpy(cfg.mqttLogin, webServer.arg("login").c_str(), 15);
    cfg.mqttLogin[15] = 0;
  }
  if (webServer.hasArg("pass") && webServer.arg("pass").length() > 0) {
    strncpy(cfg.mqttPass, webServer.arg("pass").c_str(), 15);
    cfg.mqttPass[15] = 0;
  }

  EE_updCfg();

  mqttClient.disconnect();
  setupMQTT();
  lastMqttReconnect = 0;

  webServer.sendHeader("Location", "/mqtt");
  webServer.send(302, "text/plain", "Saved");
}

void handleMqttLog() {
  if (!checkWebAuth()) return;
  String out;
  out.reserve(MQTT_LOG_SIZE * MQTT_LOG_LEN);
  byte count = mqttLogCount < MQTT_LOG_SIZE ? mqttLogCount : MQTT_LOG_SIZE;
  byte start = (mqttLogIdx - count + MQTT_LOG_SIZE) % MQTT_LOG_SIZE;
  for (byte i = 0; i < count; i++) {
    byte idx = (start + i) % MQTT_LOG_SIZE;
    out += mqttLog[idx];
    out += '\n';
  }
  webServer.send(200, "text/plain", out);
}

void setupWebOTA() {
  webServer.on("/", HTTP_GET, handleStatusPage);

  webServer.on("/mqtt", HTTP_GET, handleMqttPage);
  webServer.on("/mqtt", HTTP_POST, handleMqttSave);
  webServer.on("/mqtt/log", HTTP_GET, handleMqttLog);

  webServer.on("/update", HTTP_GET, handleOTAPage);

  webServer.on("/update", HTTP_POST, []() {
    bool success = !Update.hasError();
    String response = success ? "{\"ok\":true}" : "{\"ok\":false}";
    webServer.send(200, "application/json", response);
    if (success) {
      delay(1000);
      ESP.restart();
    }
  }, []() {
    if (!webServer.authenticate(cfg.webUser, cfg.webPass)) return;
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      DEBUGLN("OTA Upload: " + upload.filename);
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) {
        DEBUGLN("OTA begin failed");
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        DEBUGLN("OTA write failed");
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        cfg.update = 1;
        cfg.version = GL_VERSION;
        EE_updCfg();
        DEBUGLN("OTA Success");
      } else {
        DEBUGLN("OTA End failed");
      }
    }
    yield();
  });

  webServer.begin();

  DEBUG("Web OTA ready. User: ");
  DEBUG(cfg.webUser);
  DEBUG(" Pass: ");
  DEBUGLN(cfg.webPass);
}
