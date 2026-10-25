// ==================== MQTT + HOME ASSISTANT ====================

char mqttTopicState[40];
char mqttTopicSet[40];
char mqttTopicAvail[48];
char mqttTopicPresence[48];
char mqttTopicTimezone[48];
char mqttTopicDiscovery[60];
char mqttClientId[20];

static uint32_t lastMqttReconnect = 0;

const char* effectNames[] = {
  "Perlin", "Color", "ColorChange", "Gradient", "Particles",
  "Fire", "Fire2020", "Confetti", "Vortex", "Clock", "Weather"
};
const byte effectCount = 11;

void buildMqttTopics() {
  snprintf(mqttClientId, sizeof(mqttClientId), "gyverlamp2_%s", deviceId);
  snprintf(mqttTopicState, sizeof(mqttTopicState), "gyverlamp2_%s/state", deviceId);
  snprintf(mqttTopicSet, sizeof(mqttTopicSet), "gyverlamp2_%s/set", deviceId);
  snprintf(mqttTopicAvail, sizeof(mqttTopicAvail), "gyverlamp2_%s/availability", deviceId);
  snprintf(mqttTopicPresence, sizeof(mqttTopicPresence), "gyverlamp2_%s/presence", deviceId);
  snprintf(mqttTopicTimezone, sizeof(mqttTopicTimezone), "gyverlamp2_%s/timezone", deviceId);
  snprintf(mqttTopicDiscovery, sizeof(mqttTopicDiscovery), "homeassistant/light/gyverlamp2_%s/config", deviceId);
}

void setupMQTT() {
  if (!cfg.mqtt || cfg.mqttHost[0] == 0) return;
  buildMqttTopics();
  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort > 0 ? cfg.mqttPort : 1883);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(60);
  DEBUGLN("MQTT configured");
  DEBUG("  Host: "); DEBUGLN(cfg.mqttHost);
  DEBUG("  Port: "); DEBUGLN(cfg.mqttPort > 0 ? cfg.mqttPort : 1883);
  DEBUG("  Client: "); DEBUGLN(mqttClientId);
}

bool mqttReconnect() {
  if (!cfg.mqtt || cfg.mqttHost[0] == 0) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  DEBUG("MQTT connecting... ");
  bool connected;
  if (cfg.mqttLogin[0] != 0) {
    connected = mqttClient.connect(mqttClientId, cfg.mqttLogin, cfg.mqttPass,
      mqttTopicAvail, 0, true, "offline");
  } else {
    connected = mqttClient.connect(mqttClientId, NULL, NULL,
      mqttTopicAvail, 0, true, "offline");
  }

  if (connected) {
    DEBUGLN("OK");
    mqttClient.publish(mqttTopicAvail, "online", true);
    mqttClient.subscribe(mqttTopicSet);
    char subTopic[45];
    snprintf(subTopic, sizeof(subTopic), "%s/+", mqttTopicSet);
    mqttClient.subscribe(subTopic);
    mqttClient.subscribe(mqttTopicPresence);
    mqttClient.subscribe(mqttTopicTimezone);
    if (cfg.mqttAutoDisc) mqttPublishDiscovery();
    mqttPublishState();
    return true;
  }

  DEBUG("FAIL rc="); DEBUGLN(mqttClient.state());
  return false;
}

void mqttLoop() {
  if (!cfg.mqtt || cfg.mqttHost[0] == 0) return;
  if (!mqttClient.connected()) {
    uint32_t now = millis();
    if (now - lastMqttReconnect > 30000) {
      lastMqttReconnect = now;
      mqttReconnect();
    }
  } else {
    mqttClient.loop();
  }
}

void handleMqttSubCommand(const char* param, const char* msg) {
  int val = atoi(msg);
  bool cfgChanged = false;
  bool presetChanged = false;

  if (strcmp(param, "speed") == 0) { CUR_PRES.speed = constrain(val, 0, 255); presetChanged = true; }
  else if (strcmp(param, "scale") == 0) { CUR_PRES.scale = constrain(val, 0, 255); presetChanged = true; }
  else if (strcmp(param, "color") == 0) { CUR_PRES.color = constrain(val, 0, 255); presetChanged = true; }
  else if (strcmp(param, "palette") == 0) { CUR_PRES.palette = constrain(val, 1, 16); presetChanged = true; }
  else if (strcmp(param, "from_center") == 0) { CUR_PRES.fromCenter = (strcmp(msg, "ON") == 0) ? 1 : 0; presetChanged = true; }
  else if (strcmp(param, "preset") == 0) {
    setPreset(constrain(val, 0, cfg.presetAmount - 1));
    EE_updateCfg();
    return;
  }
  else if (strcmp(param, "work_from") == 0) { cfg.workFrom = constrain(val, 0, 23); cfgChanged = true; }
  else if (strcmp(param, "work_to") == 0) { cfg.workTo = constrain(val, 0, 23); cfgChanged = true; }
  else if (strcmp(param, "rotation") == 0) { cfg.rotation = (strcmp(msg, "ON") == 0) ? 1 : 0; cfgChanged = true; }
  else if (strcmp(param, "presence") == 0) { cfg.presenceEnabled = (strcmp(msg, "ON") == 0) ? 1 : 0; cfgChanged = true; }
  else return;

  if (cfgChanged) EE_updateCfg();
  if (presetChanged) EE_updatePreset();
  mqttPublishState();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char msg[128];
  uint16_t len = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
  memcpy(msg, payload, len);
  msg[len] = 0;

  DEBUG("MQTT << "); DEBUG(topic); DEBUG(" : "); DEBUGLN(msg);

  if (strcmp(topic, mqttTopicPresence) == 0) {
    handlePresenceMessage((byte*)msg, len);
    return;
  }

  if (strcmp(topic, mqttTopicTimezone) == 0) {
    long offsetSec = atol(msg);
    byte gmtVal = (byte)constrain((int)(offsetSec / 3600) + 13, 0, 26);
    if (gmtVal != cfg.GMT) {
      cfg.GMT = gmtVal;
      ntp.setTimeOffset((cfg.GMT - 13) * 3600l);
      EE_updateCfg();
      DEBUGLN("Timezone updated via MQTT");
    }
    return;
  }

  byte setTopicLen = strlen(mqttTopicSet);
  if (strncmp(topic, mqttTopicSet, setTopicLen) == 0 && topic[setTopicLen] == '/') {
    handleMqttSubCommand(topic + setTopicLen + 1, msg);
    return;
  }

  if (strcmp(topic, mqttTopicSet) != 0) return;

  char* stateStr = strstr(msg, "\"state\"");
  if (stateStr) {
    if (strstr(stateStr, "\"ON\"")) { if (!cfg.state) controlHandler(true); }
    else if (strstr(stateStr, "\"OFF\"")) { if (cfg.state) controlHandler(false); }
  }

  char* briStr = strstr(msg, "\"brightness\"");
  if (briStr) {
    char* colon = strchr(briStr + 12, ':');
    if (colon) {
      int bri = atoi(colon + 1);
      if (bri >= 0 && bri <= 255) {
        cfg.bright = bri;
        EE_updateCfg();
        mqttPublishState();
      }
    }
  }

  char* effStr = strstr(msg, "\"effect\"");
  if (effStr) {
    char* quote1 = strchr(effStr + 8, '"');
    if (quote1) {
      quote1++;
      char* quote2 = strchr(quote1, '"');
      if (quote2) {
        *quote2 = 0;
        for (byte i = 0; i < effectCount; i++) {
          if (strcmp(quote1, effectNames[i]) == 0) {
            setPreset(i);
            EE_updateCfg();
            break;
          }
        }
      }
    }
  }
}

void mqttPublishState() {
  if (!cfg.mqtt || !mqttClient.connected()) return;

  byte effIdx = constrain(cfg.curPreset, 0, effectCount - 1);
  char payload[300];
  snprintf(payload, sizeof(payload),
    "{\"state\":\"%s\",\"brightness\":%d,\"effect\":\"%s\",\"preset\":%d,"
    "\"speed\":%d,\"scale\":%d,\"color\":%d,\"palette\":%d,"
    "\"from_center\":\"%s\",\"rotation\":\"%s\","
    "\"work_from\":%d,\"work_to\":%d,\"presence\":\"%s\"}",
    cfg.state ? "ON" : "OFF",
    cfg.bright,
    effectNames[effIdx],
    cfg.curPreset,
    CUR_PRES.speed,
    CUR_PRES.scale,
    CUR_PRES.color,
    CUR_PRES.palette,
    CUR_PRES.fromCenter ? "ON" : "OFF",
    cfg.rotation ? "ON" : "OFF",
    cfg.workFrom,
    cfg.workTo,
    cfg.presenceEnabled ? "ON" : "OFF"
  );

  mqttClient.publish(mqttTopicState, payload, true);
}

// ---- HA Auto-Discovery helpers ----

void mqttDiscNumber(const char* name, const char* field, int minV, int maxV, const char* icon) {
  char topic[80];
  char payload[480];
  snprintf(topic, sizeof(topic), "homeassistant/number/%s_%s/config", mqttClientId, field);
  snprintf(payload, sizeof(payload),
    "{\"name\":\"%s\",\"uniq_id\":\"%s_%s\","
    "\"stat_t\":\"%s\",\"cmd_t\":\"%s/%s\","
    "\"val_tpl\":\"{{value_json.%s}}\","
    "\"min\":%d,\"max\":%d,"
    "\"icon\":\"%s\","
    "\"avty_t\":\"%s\","
    "\"dev\":{\"ids\":\"%s\"}}",
    name, mqttClientId, field,
    mqttTopicState, mqttTopicSet, field,
    field,
    minV, maxV,
    icon,
    mqttTopicAvail,
    mqttClientId
  );
  mqttClient.publish(topic, payload, true);
  yield();
}

void mqttDiscSwitch(const char* name, const char* field, const char* icon) {
  char topic[80];
  char payload[480];
  snprintf(topic, sizeof(topic), "homeassistant/switch/%s_%s/config", mqttClientId, field);
  snprintf(payload, sizeof(payload),
    "{\"name\":\"%s\",\"uniq_id\":\"%s_%s\","
    "\"stat_t\":\"%s\",\"cmd_t\":\"%s/%s\","
    "\"val_tpl\":\"{{value_json.%s}}\","
    "\"icon\":\"%s\","
    "\"avty_t\":\"%s\","
    "\"dev\":{\"ids\":\"%s\"}}",
    name, mqttClientId, field,
    mqttTopicState, mqttTopicSet, field,
    field,
    icon,
    mqttTopicAvail,
    mqttClientId
  );
  mqttClient.publish(topic, payload, true);
  yield();
}

void mqttPublishDiscovery() {
  char payload[480];
  int pos = snprintf(payload, sizeof(payload),
    "{\"name\":\"GyverLamp2 %s\","
    "\"uniq_id\":\"%s\","
    "\"cmd_t\":\"%s\","
    "\"stat_t\":\"%s\","
    "\"avty_t\":\"%s\","
    "\"schema\":\"json\","
    "\"brightness\":true,"
    "\"effect\":true,"
    "\"effect_list\":[",
    deviceId, mqttClientId, mqttTopicSet, mqttTopicState, mqttTopicAvail
  );

  for (byte i = 0; i < effectCount && pos < (int)sizeof(payload) - 60; i++) {
    if (i > 0) pos += snprintf(payload + pos, sizeof(payload) - pos, ",");
    pos += snprintf(payload + pos, sizeof(payload) - pos, "\"%s\"", effectNames[i]);
  }

  snprintf(payload + pos, sizeof(payload) - pos,
    "],\"dev\":{\"ids\":\"%s\",\"name\":\"GyverLamp2 %s\"}}",
    mqttClientId, deviceId
  );

  mqttClient.publish(mqttTopicDiscovery, payload, true);
  yield();

  mqttDiscNumber("Speed", "speed", 0, 255, "mdi:speedometer");
  mqttDiscNumber("Scale", "scale", 0, 255, "mdi:resize");
  mqttDiscNumber("Color", "color", 0, 255, "mdi:palette");
  mqttDiscNumber("Preset", "preset", 0, 39, "mdi:format-list-numbered");
  mqttDiscNumber("Work From", "work_from", 0, 23, "mdi:clock-start");
  mqttDiscNumber("Work To", "work_to", 0, 23, "mdi:clock-end");

  mqttDiscSwitch("Rotation", "rotation", "mdi:rotate-3d-variant");
  mqttDiscSwitch("From Center", "from_center", "mdi:arrow-expand-all");
  mqttDiscSwitch("Presence", "presence", "mdi:home-account");

  DEBUGLN("MQTT HA discovery published");
}
