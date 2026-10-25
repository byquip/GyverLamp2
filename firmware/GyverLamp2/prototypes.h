// Forward declarations for all .ino functions
// Needed when Arduino IDE prototype auto-generation fails

// 0_func.ino
void sendUDP(char *data);
void sendUDP(byte cmd, int data1 = 0, int data2 = 0, int data3 = 0);
void iAmOnline();
void restartUDP();
void blink16(CRGB color);
void drawDigit(byte digit, int X, int Y, CRGB color);
void drawDots(int X, int Y, CRGB color);
void drawClock(byte Y, byte speed, CRGB color);

// analog.ino
void setupADC();
void checkAnalog();
void checkMusic();
void checkPhot();
byte getSoundVol();
void switchToMic();
void switchToPhot();
void disableADC();

// button.ino
void button();

// eeprom.ino
void EE_startup();
void EE_updateCfg();
void EE_updateDawn();
void EE_updatePreset();
void EE_updatePal();
void checkEEupdate();
void EE_updCfgRst();
void EE_updCfg();

// effects.ino
void effectsRoutine();
bool musicMode();
bool briMode();
byte getBright();
int getLength();
byte getScale();
void fillStrip(int from, int to, CRGB color);
void fillRow(int row, CRGB color);
void updPal();
byte scalePal(byte val);
void setPix(int x, int y, CRGB color);
void setPixOverlap(int x, int y, CRGB color);
void setLED(int x, CRGB color);

// fire2020.ino
void fire2020(byte scale, int len);

// fire2D.ino
void fireRoutine(byte speed);
void shiftUp();
void drawFrame(int pcnt);

// mqtt.ino
extern uint32_t lastMqttReconnect;
extern char mqttLog[][96];
extern byte mqttLogIdx;
extern byte mqttLogCount;
void mqttLogAdd(const char* msg);
void buildMqttTopics();
void setupMQTT();
bool mqttReconnect();
void mqttLoop();
void handleMqttSubCommand(const char* param, const char* msg);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttPublishState();
void mqttDiscNumber(const char* name, const char* field, int minV, int maxV, const char* icon);
void mqttDiscSwitch(const char* name, const char* field, const char* icon);
void mqttPublishDiscovery();

// ota_web.ino
bool checkWebAuth();
String getUptime();
String templateReplace(const char* html);
void handleStatusPage();
void handleOTAPage();
void handleMqttPage();
void handleMqttSave();
void handleMqttLog();
void setupWebOTA();

// parsing.ino
void parsing();
void sendToSlaves(byte data1, byte data2);

// portal.ino
void generateDeviceId(char* buf);
void generateApName(char* buf, const char* id);
void generateApPassword(char* buf);
void initWebCredentials();
void startCaptivePortal();
void handlePortalRoot();
void handlePortalScan();
void handlePortalConnect();
void portalLoop();

// presetManager.ino
void presetRotation(bool force);
void changePreset(int dir);
void setPreset(byte pres);
void controlHandler(bool state);
void fade(bool state);
void setPower(bool state);
void fadeDown(uint32_t time);

// presence.ino
void handlePresenceMessage(byte* payload, unsigned int length);
bool isPresenceTime();
void checkPresence();

// startup.ino
void checkButton();
void checkGroup();
void startStrip();
void showRGB();
void startWiFi();
void setupAP();
void setupLocal();
void checkUpdate();
void tryReconnect();
void misc();

// time.ino
void setupTime();
void timeTicker();
void sendTimeToLocals();
void setTime(byte day, byte hour, byte min, byte sec);
void checkDawn();
void checkWorkTime();
bool isWorkTime(byte t, byte from, byte to);
