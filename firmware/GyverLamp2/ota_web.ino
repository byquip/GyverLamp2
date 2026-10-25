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
<a class="btn" href="/update">Firmware Update</a>
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

void setupWebOTA() {
  webServer.on("/", HTTP_GET, handleStatusPage);

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
