// ==================== CAPTIVE PORTAL ====================

void generateDeviceId(char* buf) {
  uint32_t chipId = ESP.getChipId();
  snprintf(buf, 5, "%04X", (uint16_t)(chipId & 0xFFFF));
}

void generateApName(char* buf, const char* id) {
  snprintf(buf, 20, "GyverLamp2-%s", id);
}

void generateApPassword(char* buf) {
  uint32_t chipId = ESP.getChipId();
  uint32_t hash = chipId ^ 0xA5C3E1D7;
  hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
  hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
  hash = (hash >> 16) ^ hash;
  snprintf(buf, 9, "%08X", hash);
}

void initWebCredentials() {
  if (cfg.webPass[0] == 0) {
    strncpy(cfg.webPass, apPassword, 15);
    cfg.webPass[15] = 0;
    EE_updCfg();
    DEBUG("Web password set to: ");
    DEBUGLN(cfg.webPass);
  }
}

// -------- HTML captive portal page (PROGMEM) --------
const char PORTAL_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GyverLamp2 Setup</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#e0e0e0;padding:16px;max-width:420px;margin:0 auto}
h1{color:#e94560;margin-bottom:4px;font-size:1.4em}
.sub{color:#888;margin-bottom:16px;font-size:.85em}
.card{background:#16213e;border-radius:12px;padding:16px;margin-bottom:12px}
label{display:block;margin-bottom:4px;font-size:.9em;color:#aaa}
input[type=password],input[type=text]{width:100%;padding:10px;border:1px solid #333;border-radius:8px;background:#0f3460;color:#fff;font-size:1em;margin-bottom:8px}
button{width:100%;padding:12px;background:#e94560;color:#fff;border:none;border-radius:8px;font-size:1em;cursor:pointer;margin-top:8px}
button:hover{background:#c73650}
button:disabled{background:#555;cursor:wait}
.net{padding:10px;border-bottom:1px solid #1a1a2e;cursor:pointer;display:flex;justify-content:space-between}
.net:hover{background:#0f3460;border-radius:6px}
.signal{font-size:.8em;color:#888}
.ok{color:#4caf50;text-align:center;padding:20px;font-size:1.1em}
.err{color:#e94560;text-align:center;padding:10px;font-size:.9em}
#loader{display:none;text-align:center;padding:20px}
.spin{border:3px solid #333;border-top:3px solid #e94560;border-radius:50%;width:30px;height:30px;animation:spin 1s linear infinite;margin:0 auto 10px}
@keyframes spin{to{transform:rotate(360deg)}}
</style></head><body>
<h1>GyverLamp2</h1>
<div class="sub" id="subtitle"></div>
<div class="card" id="scanCard">
<div id="nets"></div>
<button onclick="scan()" id="scanBtn"></button>
</div>
<div class="card" id="connCard" style="display:none">
<div id="selNet" style="font-weight:bold;margin-bottom:8px"></div>
<label id="lPass"></label>
<input type="password" id="pass">
<button onclick="connect()" id="connBtn"></button>
<button onclick="back()" id="backBtn" style="background:#333;margin-top:6px"></button>
</div>
<div id="loader"><div class="spin"></div><span id="loadTxt"></span></div>
<div id="result"></div>
<script>
var L=navigator.language&&navigator.language.startsWith('ru')?'ru':'en';
var T={
subtitle:{ru:'Настройка Wi-Fi',en:'Wi-Fi Setup'},
scan:{ru:'Сканировать сети',en:'Scan Networks'},
scanning:{ru:'Сканирование...',en:'Scanning...'},
noNets:{ru:'Сети не найдены',en:'No networks found'},
pass:{ru:'Пароль Wi-Fi:',en:'WiFi Password:'},
connect:{ru:'Подключить',en:'Connect'},
back:{ru:'Назад',en:'Back'},
connecting:{ru:'Подключение...',en:'Connecting...'},
ok:{ru:'Подключено! IP: ',en:'Connected! IP: '},
fail:{ru:'Ошибка подключения. Попробуйте снова.',en:'Connection failed. Try again.'},
reboot:{ru:'Лампа перезагружается...',en:'Lamp is rebooting...'}
};
function t(k){return T[k][L]||T[k].en}
document.getElementById('subtitle').textContent=t('subtitle');
document.getElementById('scanBtn').textContent=t('scan');
document.getElementById('lPass').textContent=t('pass');
document.getElementById('connBtn').textContent=t('connect');
document.getElementById('backBtn').textContent=t('back');
var selSSID='';
function scan(){
  document.getElementById('scanBtn').disabled=true;
  document.getElementById('scanBtn').textContent=t('scanning');
  fetch('/scan').then(r=>r.json()).then(d=>{
    var h='';
    if(d.length==0)h='<div class="err">'+t('noNets')+'</div>';
    else d.forEach(n=>{
      h+='<div class="net" onclick="sel(\''+n.s.replace(/'/g,"\\'")+'\')">'
        +'<span>'+n.s+(n.e?' 🔒':'')+'</span>'
        +'<span class="signal">'+n.r+' dBm</span></div>';
    });
    document.getElementById('nets').innerHTML=h;
    document.getElementById('scanBtn').disabled=false;
    document.getElementById('scanBtn').textContent=t('scan');
  }).catch(()=>{
    document.getElementById('scanBtn').disabled=false;
    document.getElementById('scanBtn').textContent=t('scan');
  });
}
function sel(s){
  selSSID=s;
  document.getElementById('selNet').textContent=s;
  document.getElementById('scanCard').style.display='none';
  document.getElementById('connCard').style.display='block';
  document.getElementById('pass').value='';
  document.getElementById('pass').focus();
}
function back(){
  document.getElementById('scanCard').style.display='block';
  document.getElementById('connCard').style.display='none';
}
function connect(){
  var p=document.getElementById('pass').value;
  document.getElementById('connCard').style.display='none';
  document.getElementById('loader').style.display='block';
  document.getElementById('loadTxt').textContent=t('connecting');
  fetch('/connect',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'ssid='+encodeURIComponent(selSSID)+'&pass='+encodeURIComponent(p)})
  .then(r=>r.json()).then(d=>{
    document.getElementById('loader').style.display='none';
    if(d.ok){
      document.getElementById('result').innerHTML='<div class="ok">'+t('ok')+d.ip+'<br><br>'+t('reboot')+'</div>';
    }else{
      document.getElementById('result').innerHTML='<div class="err">'+t('fail')+'</div>';
      setTimeout(()=>{
        document.getElementById('result').innerHTML='';
        document.getElementById('scanCard').style.display='block';
      },3000);
    }
  }).catch(()=>{
    document.getElementById('loader').style.display='none';
    document.getElementById('result').innerHTML='<div class="ok">'+t('reboot')+'</div>';
  });
}
scan();
</script></body></html>)rawliteral";

void startCaptivePortal() {
  DEBUGLN("Starting captive portal");
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  delay(100);
  WiFi.softAP(apName, apPassword);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", WiFi.softAPIP());

  webServer.on("/", HTTP_GET, handlePortalRoot);
  webServer.on("/scan", HTTP_GET, handlePortalScan);
  webServer.on("/connect", HTTP_POST, handlePortalConnect);
  webServer.on("/generate_204", HTTP_GET, handlePortalRoot);
  webServer.on("/fwlink", HTTP_GET, handlePortalRoot);
  webServer.onNotFound(handlePortalRoot);
  webServer.begin();

  captivePortalActive = true;
  blink16(CRGB::Yellow);

  DEBUG("Captive Portal AP: ");
  DEBUGLN(apName);
  DEBUG("AP Password: ");
  DEBUGLN(apPassword);
  DEBUG("Portal IP: ");
  DEBUGLN(WiFi.softAPIP());
}

void handlePortalRoot() {
  webServer.send_P(200, "text/html", PORTAL_HTML);
}

void handlePortalScan() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{\"s\":\"";
    String ssid = WiFi.SSID(i);
    ssid.replace("\"", "\\\"");
    json += ssid;
    json += "\",\"r\":";
    json += WiFi.RSSI(i);
    json += ",\"e\":";
    json += (WiFi.encryptionType(i) != ENC_TYPE_NONE) ? "true" : "false";
    json += "}";
  }
  json += "]";
  WiFi.scanDelete();
  webServer.send(200, "application/json", json);
}

void handlePortalConnect() {
  String ssid = webServer.arg("ssid");
  String pass = webServer.arg("pass");

  if (ssid.length() == 0 || ssid.length() > 31) {
    webServer.send(200, "application/json", "{\"ok\":false}");
    return;
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  uint32_t tmr = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - tmr < 15000) {
    delay(200);
    yield();
  }

  if (WiFi.status() == WL_CONNECTED) {
    strncpy(cfg.ssid, ssid.c_str(), 31);
    cfg.ssid[31] = 0;
    strncpy(cfg.pass, pass.c_str(), 31);
    cfg.pass[31] = 0;
    cfg.WiFimode = 1;
    EE_updCfg();

    String response = "{\"ok\":true,\"ip\":\"";
    response += WiFi.localIP().toString();
    response += "\"}";
    webServer.send(200, "application/json", response);

    DEBUG("Connected! IP: ");
    DEBUGLN(WiFi.localIP());

    delay(2000);
    ESP.restart();
  } else {
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apName, apPassword);
    webServer.send(200, "application/json", "{\"ok\":false}");
  }
}

void portalLoop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  yield();
}
