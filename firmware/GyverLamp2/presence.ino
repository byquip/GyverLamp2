// ==================== PRESENCE DETECTION (via MQTT from Home Assistant) ====================

static bool presenceAutoOn = false;

void handlePresenceMessage(byte* payload, unsigned int length) {
  char msg[32];
  uint16_t len = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
  memcpy(msg, payload, len);
  msg[len] = 0;

  bool wasHome = cfg.presenceHome;

  if (strstr(msg, "home") || strstr(msg, "true") || strstr(msg, "\"ON\"") || strcmp(msg, "1") == 0) {
    cfg.presenceHome = 1;
    DEBUGLN("Presence: HOME");
  } else if (strstr(msg, "away") || strstr(msg, "false") || strstr(msg, "\"OFF\"") || strcmp(msg, "0") == 0) {
    cfg.presenceHome = 0;
    DEBUGLN("Presence: AWAY");
  }

  if (wasHome && !cfg.presenceHome) {
    cfg.manualOff = 0;
    presenceAutoOn = false;
  }

  checkPresence();
}

bool isPresenceTime() {
  if (!gotNTP && !gotTime) return false;
  return isWorkTime(now.hour, cfg.workFrom, cfg.workTo);
}

void checkPresence() {
  if (!cfg.presenceEnabled) return;

  if (cfg.presenceHome && isPresenceTime()) {
    if (!cfg.state && !cfg.manualOff) {
      DEBUGLN("Presence -> ON");
      fade(1);
      presenceAutoOn = true;
    }
  }

  if (!cfg.presenceHome) {
    if (cfg.state && presenceAutoOn && !cfg.manualOff) {
      DEBUGLN("Presence -> OFF");
      fade(0);
      presenceAutoOn = false;
    }
  }
}
