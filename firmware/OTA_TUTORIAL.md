# GyverLamp2 — Инструкция по прошивке / Firmware Guide

---

## RU: Первая прошивка через USB (обязательно один раз)

После этого все обновления можно делать по воздуху (WiFi).

### 1. Установите Arduino IDE
1. Скачайте и установите [Arduino IDE](https://www.arduino.cc/en/software) (версия 1.8.x или 2.x)
2. Откройте **Файл → Настройки**
3. В поле **"Дополнительные ссылки для Менеджера плат"** добавьте:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Нажмите OK

### 2. Установите поддержку ESP8266
1. Откройте **Инструменты → Плата → Менеджер плат**
2. Найдите **"esp8266"** (by ESP8266 Community)
3. Установите версию **2.7.4** или новее
4. Дождитесь окончания установки

### 3. Установите библиотеку FastLED
1. Откройте **Скетч → Подключить библиотеку → Управлять библиотеками**
2. Найдите **"FastLED"**
3. Установите версию **3.4.0** или новее

### 4. Подключите библиотеку PubSubClient
Библиотека PubSubClient уже включена в проект в папке `libraries/PubSubClient/`.
Arduino IDE подхватит её автоматически, если вы открываете проект из его папки.

Если IDE не находит библиотеку:
1. Скопируйте папку `libraries/PubSubClient/` в вашу папку библиотек Arduino
   - Windows: `C:\Users\<ваше_имя>\Documents\Arduino\libraries\`
   - macOS: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`

### 5. Подключите ESP8266 к компьютеру
1. Подключите плату (NodeMCU/Wemos D1 Mini) к компьютеру через USB-кабель
2. Если компьютер не видит плату, установите драйвер:
   - **CH340** (большинство китайских плат): [скачать](https://www.wch-ic.com/downloads/CH341SER_EXE.html)
   - **CP2102** (некоторые платы): [скачать](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
3. После установки драйвера должен появиться COM-порт в диспетчере устройств

### 6. Настройте Arduino IDE
1. **Инструменты → Плата** → выберите **Generic ESP8266 Module**
2. **Инструменты → Flash Size** → **4MB (FS:2MB OTA:~1019KB)**
3. **Инструменты → CPU Frequency** → **160 MHz**
4. **Инструменты → Upload Speed** → **115200** (или 921600, если работает стабильно)
5. **Инструменты → Порт** → выберите COM-порт вашей платы
6. **Инструменты → Erase Flash** → **All Flash Contents** (только при первой прошивке!)

### 7. Загрузите прошивку
1. Откройте файл `firmware/GyverLamp2/GyverLamp2.ino` в Arduino IDE
2. Нажмите кнопку **"Загрузка"** (→) или **Скетч → Загрузка**
3. Дождитесь завершения компиляции и загрузки
4. В нижней строке появится **"Done uploading"**

### 8. Первый запуск
1. Откройте **Инструменты → Монитор порта** (скорость **115200**)
2. Лампа создаст точку доступа WiFi: **GyverLamp2-XXXX**
3. В мониторе вы увидите:
   - **AP Name**: имя точки доступа
   - **AP Password**: пароль (уникальный, запишите!)
   - **Web password**: пароль для веб-интерфейса (совпадает с AP)
4. Подключитесь к WiFi лампы с телефона или компьютера
5. Откроется страница настройки → выберите вашу домашнюю WiFi-сеть
6. Лампа перезагрузится и подключится к вашему WiFi
7. В мониторе порта появится IP-адрес лампы

**Готово! Теперь все обновления можно делать через браузер (см. ниже).**

### Устранение проблем при USB-прошивке
- **Ошибка компиляции**: убедитесь что установлены ESP8266 core и FastLED нужных версий
- **COM-порт не появляется**: установите драйвер CH340 или CP2102
- **Ошибка загрузки "espcomm_upload_mem failed"**: попробуйте уменьшить Upload Speed до 115200
- **"fatal error: PubSubClient.h: No such file"**: скопируйте `libraries/PubSubClient/` в папку библиотек Arduino
- **Лампа не создаёт WiFi**: проверьте монитор порта на ошибки, убедитесь что Erase Flash = All Flash Contents

---

## EN: First USB Flash (required once)

After this, all updates can be done over WiFi.

### 1. Install Arduino IDE
1. Download and install [Arduino IDE](https://www.arduino.cc/en/software) (1.8.x or 2.x)
2. Open **File → Preferences**
3. Add to **"Additional Boards Manager URLs"**:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Click OK

### 2. Install ESP8266 Board Support
1. Open **Tools → Board → Boards Manager**
2. Search for **"esp8266"** (by ESP8266 Community)
3. Install version **2.7.4** or newer

### 3. Install FastLED Library
1. Open **Sketch → Include Library → Manage Libraries**
2. Search for **"FastLED"**
3. Install version **3.4.0** or newer

### 4. PubSubClient Library
PubSubClient is included in the project under `libraries/PubSubClient/`.
Arduino IDE picks it up automatically when you open the project from its folder.

If the IDE can't find it, copy `libraries/PubSubClient/` to your Arduino libraries folder:
- Windows: `C:\Users\<you>\Documents\Arduino\libraries\`
- macOS: `~/Documents/Arduino/libraries/`
- Linux: `~/Arduino/libraries/`

### 5. Connect ESP8266 via USB
1. Connect the board (NodeMCU/Wemos D1 Mini) to your computer via USB
2. If the computer doesn't detect it, install the driver:
   - **CH340** (most Chinese boards): [download](https://www.wch-ic.com/downloads/CH341SER_EXE.html)
   - **CP2102** (some boards): [download](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
3. A COM port should appear in Device Manager

### 6. Configure Arduino IDE
1. **Tools → Board** → select **Generic ESP8266 Module**
2. **Tools → Flash Size** → **4MB (FS:2MB OTA:~1019KB)**
3. **Tools → CPU Frequency** → **160 MHz**
4. **Tools → Upload Speed** → **115200** (or 921600 if stable)
5. **Tools → Port** → select your board's COM port
6. **Tools → Erase Flash** → **All Flash Contents** (first flash only!)

### 7. Upload Firmware
1. Open `firmware/GyverLamp2/GyverLamp2.ino` in Arduino IDE
2. Click the **"Upload"** button (→) or **Sketch → Upload**
3. Wait for compilation and upload to finish
4. You'll see **"Done uploading"** at the bottom

### 8. First Boot
1. Open **Tools → Serial Monitor** (baud rate **115200**)
2. The lamp creates a WiFi access point: **GyverLamp2-XXXX**
3. In the monitor you'll see:
   - **AP Name**: access point name
   - **AP Password**: unique password (write it down!)
   - **Web password**: web interface password (same as AP password)
4. Connect to the lamp's WiFi from your phone/computer
5. A setup page opens → select your home WiFi network
6. The lamp reboots and connects to your WiFi
7. The lamp's IP address appears in Serial Monitor

**Done! All future updates can be done via browser (see below).**

### USB Flash Troubleshooting
- **Compilation error**: make sure ESP8266 core and FastLED are the correct versions
- **No COM port**: install CH340 or CP2102 driver
- **Upload error "espcomm_upload_mem failed"**: try lowering Upload Speed to 115200
- **"fatal error: PubSubClient.h: No such file"**: copy `libraries/PubSubClient/` to Arduino libraries folder
- **Lamp doesn't create WiFi**: check Serial Monitor for errors, make sure Erase Flash = All Flash Contents

---

## RU: Обновление прошивки через браузер (OTA)

### Что вам нужно
- Компьютер или телефон в той же WiFi-сети, что и лампа
- Скомпилированный файл прошивки (.bin)

### Как скомпилировать .bin файл
1. Откройте проект в Arduino IDE
2. Выберите плату: **Инструменты → Плата → Generic ESP8266 Module**
3. Flash Size: **4MB (FS:2MB OTA)**
4. CPU Frequency: **160 MHz**
5. Нажмите **Скетч → Экспорт скомпилированного бинарника**
6. Файл `.bin` появится в папке проекта

### Как загрузить прошивку
1. **Узнайте IP-адрес лампы** одним из способов:
   - Посмотрите в Serial Monitor при загрузке (строка `Connected! Local IP:`)
   - Используйте mDNS: откройте `http://gyverlamp2-XXXX.local` (где XXXX — ID вашей лампы)
   - Посмотрите в настройках роутера (список DHCP клиентов)

2. **Откройте страницу обновления** в браузере:
   ```
   http://<IP-адрес-лампы>/update
   ```
   Например: `http://192.168.1.100/update`

3. **Введите логин и пароль:**
   - Логин по умолчанию: `admin`
   - Пароль: уникальный для каждой лампы, выводится в Serial Monitor при первом запуске
   - Пароль также совпадает с паролем точки доступа лампы

4. **Выберите .bin файл** и нажмите **"Загрузить прошивку"**

5. **Дождитесь завершения** — полоса прогресса покажет ход загрузки

6. **Лампа перезагрузится автоматически** с новой прошивкой

### Важно
- Не выключайте лампу во время обновления!
- Если обновление не удалось, лампа останется на старой прошивке
- После обновления все настройки сохраняются (WiFi, эффекты, расписание)
- Если сменился `EE_KEY` в новой прошивке, настройки сбросятся

### Устранение проблем
- **Не могу найти IP**: перезагрузите лампу и посмотрите Serial Monitor
- **Не открывается страница**: убедитесь что компьютер в той же WiFi-сети
- **Ошибка загрузки**: проверьте что файл .bin не повреждён и подходит для ESP8266
- **Забыли пароль**: зажмите кнопку при включении для сброса в режим AP, пароль AP = пароль веб-интерфейса

---

## EN: Firmware Update via Browser (OTA)

### What You Need
- A computer or phone on the same WiFi network as the lamp
- A compiled firmware file (.bin)

### How to Compile .bin File
1. Open the project in Arduino IDE
2. Select board: **Tools → Board → Generic ESP8266 Module**
3. Flash Size: **4MB (FS:2MB OTA)**
4. CPU Frequency: **160 MHz**
5. Click **Sketch → Export Compiled Binary**
6. The `.bin` file will appear in the project folder

### How to Upload Firmware
1. **Find the lamp's IP address** using one of these methods:
   - Check Serial Monitor on boot (line `Connected! Local IP:`)
   - Use mDNS: open `http://gyverlamp2-XXXX.local` (where XXXX is your lamp's ID)
   - Check your router's DHCP client list

2. **Open the update page** in a browser:
   ```
   http://<lamp-ip-address>/update
   ```
   Example: `http://192.168.1.100/update`

3. **Enter login credentials:**
   - Default username: `admin`
   - Password: unique per lamp, printed to Serial Monitor on first boot
   - Password matches the lamp's AP password

4. **Select the .bin file** and click **"Upload Firmware"**

5. **Wait for completion** — the progress bar shows upload status

6. **The lamp reboots automatically** with the new firmware

### Important
- Do NOT power off the lamp during update!
- If the update fails, the lamp stays on the old firmware
- All settings are preserved after update (WiFi, effects, schedule)
- If `EE_KEY` changed in the new firmware, settings will be reset

### Troubleshooting
- **Can't find IP**: reboot the lamp and check Serial Monitor
- **Page doesn't open**: make sure your device is on the same WiFi network
- **Upload error**: verify the .bin file is valid and built for ESP8266
- **Forgot password**: hold button during power-on to enter AP mode; AP password = web password

---

## Home Assistant Setup / Настройка Home Assistant

### MQTT Configuration
1. Install an MQTT broker (e.g., Mosquitto) on your HA server
2. Send MQTT settings to the lamp via the mobile app or configure in the web interface
3. Required settings: MQTT host, port (default: 1883), login, password
4. Enable MQTT in lamp config (`cfg.mqtt = 1`)

### Auto-Discovery
The lamp automatically publishes HA discovery configs. It appears as a **device** with:

**Light entity** (on/off, brightness, 11 effects):
- Perlin, Color, ColorChange, Gradient, Particles, Fire, Fire2020, Confetti, Vortex, Clock, Weather

**Number entities** (sliders):
- Speed (0–255), Scale (0–255), Color/Hue (0–255)
- Preset (0–39), Work From (0–23h), Work To (0–23h)

**Switch entities**:
- Rotation (auto preset change), From Center, Presence Detection

### MQTT Topics / Топики MQTT

All topics use prefix `gyverlamp2_XXXX` where XXXX = device ID.

| Topic | Direction | Description |
|-------|-----------|-------------|
| `.../state` | Lamp → HA | Full JSON state (retained) |
| `.../set` | HA → Lamp | JSON commands: `{"state":"ON"}`, `{"brightness":128}`, `{"effect":"Fire"}` |
| `.../set/speed` | HA → Lamp | Plain value `0`–`255` |
| `.../set/scale` | HA → Lamp | Plain value `0`–`255` |
| `.../set/color` | HA → Lamp | Plain value `0`–`255` |
| `.../set/palette` | HA → Lamp | Plain value `1`–`16` |
| `.../set/preset` | HA → Lamp | Plain value `0`–`39` |
| `.../set/work_from` | HA → Lamp | Hour `0`–`23` |
| `.../set/work_to` | HA → Lamp | Hour `0`–`23` |
| `.../set/rotation` | HA → Lamp | `ON` / `OFF` |
| `.../set/from_center` | HA → Lamp | `ON` / `OFF` |
| `.../set/presence` | HA → Lamp | `ON` / `OFF` |
| `.../presence` | HA → Lamp | `home` / `away` (from automation) |
| `.../timezone` | HA → Lamp | UTC offset in seconds (e.g., `10800` for UTC+3) |
| `.../availability` | Lamp → HA | `online` / `offline` (LWT) |

### Timezone & DST / Часовой пояс и летнее время

The lamp receives its timezone from Home Assistant via MQTT, so DST changes are handled automatically. Add this automation:

```yaml
automation:
  - alias: "GyverLamp2 timezone sync"
    trigger:
      - platform: homeassistant
        event: start
      - platform: time
        at: "03:00:00"
      - platform: time
        at: "15:00:00"
      - platform: mqtt
        topic: "gyverlamp2_XXXX/availability"
        payload: "online"
    action:
      - delay: "00:00:02"
      - service: mqtt.publish
        data:
          topic: "gyverlamp2_XXXX/timezone"
          payload: "{{ now().utcoffset().total_seconds() | int }}"
```

This pushes the correct UTC offset (including DST) on HA startup, twice daily, and when the lamp reconnects.

### Presence Detection / Обнаружение присутствия

The lamp turns on when someone is home (during work hours set in the app) and off when everyone leaves. Add these automations:

```yaml
automation:
  - alias: "GyverLamp2 presence - someone home"
    trigger:
      - platform: state
        entity_id: group.all_persons
        to: "home"
    action:
      - service: mqtt.publish
        data:
          topic: "gyverlamp2_XXXX/presence"
          payload: "home"

  - alias: "GyverLamp2 presence - nobody home"
    trigger:
      - platform: state
        entity_id: group.all_persons
        to: "not_home"
    action:
      - service: mqtt.publish
        data:
          topic: "gyverlamp2_XXXX/presence"
          payload: "away"
```

Replace `XXXX` with your lamp's device ID (last 4 hex digits of chip ID, shown on boot).

The lamp will:
- Turn **ON** when someone arrives home (within lamp's configured work hours)
- Turn **OFF** when everyone leaves
- Respect manual on/off overrides (manual control takes priority)
- Work hours are set via the app or HA (`work_from` / `work_to`)

### HA Dashboard / Панель управления

See `ha_config.yaml` for a ready-made Lovelace dashboard with all controls.
To set up: Settings → Dashboards → Add Dashboard → YAML mode → paste the dashboard section.

---

## First Boot / Первый запуск

1. Flash the firmware via USB
2. The lamp creates a WiFi access point: **GyverLamp2-XXXX**
3. Connect to it with your phone/computer
4. A captive portal page opens automatically
5. Select your home WiFi network and enter the password
6. The lamp connects and reboots
7. Find the lamp at `http://gyverlamp2-XXXX.local` or its IP address
8. All future firmware updates can be done over WiFi!

### Security Notes
- Each lamp has a unique AP password (derived from its chip ID)
- The web interface requires authentication
- MQTT supports username/password authentication
- Default web credentials are printed to Serial Monitor on first boot
