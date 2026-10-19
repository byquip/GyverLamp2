#if (USE_ADC == 1)
void setupADC() {
  clap.setTimeout(500);
  clap.setTrsh(250);

  vol.setDt(700);
  vol.setPeriod(5);
  vol.setWindow(map(MAX_LEDS, 300, 900, 20, 1));

  low.setDt(0);
  low.setPeriod(0);
  low.setWindow(0);
  high.setDt(0);
  high.setPeriod(0);
  high.setWindow(0);

  vol.setVolK(26);
  low.setVolK(26);
  high.setVolK(26);

  vol.setTrsh(50);
  low.setTrsh(50);
  high.setTrsh(50);

  vol.setVolMin(0);
  low.setVolMin(0);
  high.setVolMin(0);

  vol.setVolMax(255);
  low.setVolMax(255);
  high.setVolMax(255);

  phot.setDt(80);
  phot.setK(31);

  if (cfg.adcMode == GL_ADC_BRI) switchToPhot();
  else if (cfg.adcMode == GL_ADC_MIC) switchToMic();
}


void checkAnalog() {
  if (cfg.role || millis() - gotADCtmr >= 2000) {   // только мастер или слейв по таймауту опрашивает АЦП!
    switch (cfg.adcMode) {
      case GL_ADC_NONE: break;
      case GL_ADC_BRI: checkPhot(); break;
      case GL_ADC_MIC: checkMusic(); break;
      case GL_ADC_BOTH:
        {
          static timerMillis tmr(1000, 1);
          if (tmr.isReady()) {
            switchToPhot();
            phot.setRaw(analogRead(A0));
            switchToMic();
          } else {
            checkMusic();
          }
          phot.compute();
        }
        break;
    }
  }
}

void checkMusic() {


  const uint32_t nowUs = micros();
  if ((uint32_t)(nowUs - sampleTmrUs) < 1000) return;   // 1 kHz
  sampleTmrUs = nowUs;

  vol.tick();
  yield();

#if (USE_CLAP == 1)
  const int v = vol.getRawMax();
  const uint32_t nowMs = millis();

  // your work time condition
  bool work = isWorkTime(now.hour, cfg.workFrom, cfg.workTo);

  // simple smoothing (very important when lamp is ON)
  //levelFilt = (levelFilt * 7 + v) >> 3;

  const int SOUND_TRSH = 100;                // <-- tune this
  const int SOUND_TRSH_ON = 150;                // <-- tune this
  const uint16_t RETRIG_DELAY = 300;         // ms
  const uint32_t AUTO_OFF_TIME = 600000UL;//600000UL;  // 10 min
  // Serial.print("Sound clean: ");
  // Serial.print(v);
  // Serial.print(" Sound: ");
  // Serial.print(levelFilt);
  // Serial.print(" AUTO_OFF_TIME 10000: ");
  // Serial.print((uint32_t)(nowMs - lastSoundMs));
  // Serial.print("\n");
  // detect "loud enough"
  if (v >= SOUND_TRSH && (uint32_t)(nowMs - retrigMs) >= RETRIG_DELAY) {
    retrigMs = nowMs;
    lastSoundMs = nowMs;
    // Serial.print("SOUND WAS DETECTED");
    // turn ON only during allowed time
    if (!cfg.state && work && v >= SOUND_TRSH_ON) {
      Serial.println("sound -> ON");
      controlHandler(true);
      soundActivated = true;
    }
  }

  // auto OFF only if it was turned on by sound
  if (soundActivated && cfg.state && (uint32_t)(nowMs - lastSoundMs) >= AUTO_OFF_TIME) {
    Serial.println("sound timeout -> OFF");
    controlHandler(false);
    soundActivated = true;
    lastSoundMs = nowMs;
  }

  // reset flag if turned off manually
  if (!cfg.state) {
    soundActivated = true;
  }
#endif
  // if (CUR_PRES.advMode == GL_ADV_LOW || CUR_PRES.advMode == GL_ADV_HIGH) {   // частоты
  //   int raw[FFT_SIZE], spectr[FFT_SIZE];
  //   for (int i = 0; i < FFT_SIZE; i++) raw[i] = analogRead(A0);
  //   yield();
  //   FFT(raw, spectr);
  //   int low_raw = 0;
  //   int high_raw = 0;
  //   for (int i = 0; i < FFT_SIZE / 2; i++) {
  //     spectr[i] = (spectr[i] * (i + 2)) >> 1;
  //     if (i < 2) low_raw += spectr[i];
  //     else high_raw += spectr[i];
  //   }
  //   low.tick(low_raw);
  //   high.tick(high_raw);
  // }
}

void checkPhot() {
  static timerMillis tmr(1000, true);
  if (tmr.isReady()) phot.setRaw(analogRead(A0));
  phot.compute();
}

byte getSoundVol() {
  switch (CUR_PRES.advMode) {
    case GL_ADV_VOL: return vol.getVol();
    case GL_ADV_LOW: return low.getVol();
    case GL_ADV_HIGH: return high.getVol();
  }
  return 0;
}

void switchToMic() {
  digitalWrite(PHOT_VCC, 0);
  pinMode(PHOT_VCC, INPUT);
  pinMode(MIC_VCC, OUTPUT);
  digitalWrite(MIC_VCC, 1);
}
void switchToPhot() {
  digitalWrite(MIC_VCC, 0);
  pinMode(MIC_VCC, INPUT);
  pinMode(PHOT_VCC, OUTPUT);
  digitalWrite(PHOT_VCC, 1);
}
void disableADC() {
  digitalWrite(PHOT_VCC, 0);
  pinMode(PHOT_VCC, INPUT);
  digitalWrite(MIC_VCC, 0);
  pinMode(MIC_VCC, INPUT);
}
#else
void setupADC() {}
void checkAnalog() {}
void checkMusic() {}
void checkPhot() {}
byte getSoundVol() {
  return 0;
}
void switchToMic() {}
void switchToPhot() {}
void disableADC() {}
#endif
