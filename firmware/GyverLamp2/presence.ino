// ==================== PRESENCE DETECTION (via MQTT from Home Assistant) ====================

void handlePresenceMessage(byte* payload, unsigned int length) {
  char msg[32];
  uint16_t len = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
  memcpy(msg, payload, len);
  msg[len] = 0;

  if (strstr(msg, "home") || strstr(msg, "true") || strstr(msg, "\"ON\"") || strcmp(msg, "1") == 0) {
    cfg.presenceHome = 1;
    mqttLogAdd("Presence: HOME");
  } else if (strstr(msg, "away") || strstr(msg, "false") || strstr(msg, "\"OFF\"") || strcmp(msg, "0") == 0) {
    cfg.presenceHome = 0;
    mqttLogAdd("Presence: AWAY");
  }

  checkPresence();
}

bool isPresenceTime() {
  if (!gotNTP && !gotTime) return true;
  return isWorkTime(now.hour, cfg.workFrom, cfg.workTo);
}

void checkPresence() {
  if (!cfg.presenceEnabled) {
    mqttLogAdd("Presence: SKIP (disabled)");
    return;
  }

  if (cfg.presenceHome) {
    if (!cfg.state && isPresenceTime()) {
      mqttLogAdd("Presence -> ON");
      fade(1);
    }
  } else {
    if (cfg.state) {
      mqttLogAdd("Presence -> OFF");
      fade(0);
    }
  }
}
