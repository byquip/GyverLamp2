# Home Assistant + GyverLamp2: Полное руководство по настройке

---

## Шаг 1: Установка Home Assistant на NAS (Docker)

HEX OS поддерживает Docker. Home Assistant устанавливается как контейнер.

### 1.1. Откройте Docker в HEX OS

1. Зайдите в веб-интерфейс вашего NAS
2. Откройте приложение **Docker** (или **Container Manager**)
3. Если Docker не установлен — установите его из магазина приложений

### 1.2. Создайте папку для конфигурации

Создайте папку на NAS для хранения конфигурации HA:
```
/volume1/docker/homeassistant/config
```
(путь может отличаться в зависимости от NAS)

### 1.3. Установите контейнер Home Assistant

**Через интерфейс Docker:**
1. Найдите образ: `ghcr.io/home-assistant/home-assistant:stable`
2. Скачайте его
3. Создайте контейнер с параметрами:
   - **Имя**: `homeassistant`
   - **Сеть**: `host` (важно! не bridge)
   - **Том**: `/volume1/docker/homeassistant/config` → `/config`
   - **Переменная окружения**: `TZ=Europe/Paris` (ваш часовой пояс)
   - **Перезапуск**: `always`

**Или через SSH/терминал:**
```bash
docker run -d \
  --name homeassistant \
  --restart=always \
  --network=host \
  -v /volume1/docker/homeassistant/config:/config \
  -e TZ=Europe/Paris \
  ghcr.io/home-assistant/home-assistant:stable
```

> Замените `Europe/Paris` на ваш часовой пояс.
> Список: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones

### 1.4. Первый запуск HA

1. Подождите 2-3 минуты пока контейнер запустится
2. Откройте в браузере: `http://<IP-NAS>:8123`
3. Создайте учётную запись администратора
4. Укажите ваше местоположение (для часового пояса и автоматизаций)
5. Пройдите мастер первоначальной настройки

---

## Шаг 2: Установка MQTT брокера (Mosquitto)

MQTT — это протокол связи между лампой и Home Assistant.

### 2.1. Установите Mosquitto как Docker контейнер

**Создайте папки:**
```
/volume1/docker/mosquitto/config
/volume1/docker/mosquitto/data
/volume1/docker/mosquitto/log
```

**Создайте файл конфигурации** `/volume1/docker/mosquitto/config/mosquitto.conf`:
```
persistence true
persistence_location /mosquitto/data/
log_dest file /mosquitto/log/mosquitto.log

listener 1883
allow_anonymous false
password_file /mosquitto/config/password.txt
```

**Запустите контейнер:**
```bash
docker run -d \
  --name mosquitto \
  --restart=always \
  -p 1883:1883 \
  -v /volume1/docker/mosquitto/config:/mosquitto/config \
  -v /volume1/docker/mosquitto/data:/mosquitto/data \
  -v /volume1/docker/mosquitto/log:/mosquitto/log \
  eclipse-mosquitto:2
```

### 2.2. Создайте пользователя MQTT

```bash
docker exec -it mosquitto mosquitto_passwd -c /mosquitto/config/password.txt lamp_user
```
Введите пароль (например: `lamp_pass123`). Запомните логин и пароль!

Перезапустите Mosquitto:
```bash
docker restart mosquitto
```

### 2.3. Подключите MQTT к Home Assistant

1. Откройте HA: `http://<IP-NAS>:8123`
2. Перейдите **Settings** (Настройки) → **Devices & Services** (Устройства и сервисы)
3. Нажмите **Add Integration** (Добавить интеграцию)
4. Найдите **MQTT**
5. Введите:
   - **Broker**: `localhost` (или IP NAS, если Mosquitto на другой машине)
   - **Port**: `1883`
   - **Username**: `lamp_user`
   - **Password**: `lamp_pass123`
6. Нажмите **Submit**
7. Должно появиться **"Successfully connected"**

---

## Шаг 3: Настройка лампы для работы с MQTT

Лампа должна знать адрес MQTT брокера. Настройки отправляются через мобильное приложение GyverLamp2.

### 3.1. Через мобильное приложение

В приложении GyverLamp2 откройте настройки и укажите:
- **MQTT Host**: IP-адрес вашего NAS (например `192.168.1.50`)
- **MQTT Port**: `1883`
- **MQTT Login**: `lamp_user`
- **MQTT Password**: `lamp_pass123`
- **MQTT Enable**: включить

Приложение отправит настройки лампе по UDP.

### 3.2. Проверка подключения

1. После отправки настроек лампа перезагрузится
2. Откройте `http://<IP-лампы>/` в браузере (логин: admin, пароль: пароль AP)
3. В строке **MQTT** должно быть **Connected**
4. В Home Assistant перейдите **Settings** → **Devices & Services** → **MQTT**
5. Должно появиться устройство **GyverLamp2 XXXX**

> Если лампа не появляется в HA:
> - Проверьте что IP NAS правильный
> - Проверьте логин/пароль MQTT
> - Посмотрите Serial Monitor лампы для отладки

---

## Шаг 4: Автоматизации

### 4.1. Синхронизация часового пояса (обязательно!)

Лампа получает правильное время (с учётом летнего/зимнего) от HA.

1. В HA перейдите **Settings** → **Automations & Scenes** → **Create Automation**
2. Переключитесь в режим **YAML** (три точки → Edit in YAML)
3. Вставьте (замените XXXX на ID вашей лампы):

```yaml
alias: "GyverLamp2 timezone sync"
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

4. Сохраните

### 4.2. Мониторинг присутствия (опционально)

Лампа включается когда кто-то дома, выключается когда все ушли (в рабочие часы лампы).

**Сначала настройте отслеживание людей в HA:**

1. **Settings** → **People** → добавьте всех жильцов
2. Установите приложение **Home Assistant Companion** на телефоны всех жильцов
3. В приложении включите **Location tracking**
4. Убедитесь что в HA появились сущности `person.имя` со статусом `home`/`not_home`

**Затем создайте две автоматизации:**

**Автоматизация "Кто-то пришёл домой":**
```yaml
alias: "GyverLamp2 presence - someone home"
trigger:
  - platform: state
    entity_id: group.all_persons
    to: "home"
action:
  - service: mqtt.publish
    data:
      topic: "gyverlamp2_XXXX/presence"
      payload: "home"
```

**Автоматизация "Все ушли":**
```yaml
alias: "GyverLamp2 presence - nobody home"
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

> **Примечание:** `group.all_persons` — встроенная группа HA, включающая всех `person`.
> Если она не работает, создайте группу вручную в `configuration.yaml`:
> ```yaml
> group:
>   all_persons:
>     entities:
>       - person.kostya
>       - person.anna
> ```

### 4.3. Включите присутствие на лампе

В Home Assistant в карточке устройства GyverLamp2 — включите переключатель **Presence**.
Или через MQTT:
```
Topic: gyverlamp2_XXXX/set/presence
Payload: ON
```

---

## Шаг 5: Панель управления (Dashboard)

### 5.1. Быстрый способ (через UI)

1. **Settings** → **Dashboards** → **Add Dashboard**
2. Дайте имя: **GyverLamp2**
3. Откройте новый дашборд
4. Нажмите три точки → **Edit Dashboard** → **Add Card**
5. Добавьте:
   - **Light** card → выберите `light.gyverlamp2_xxxx`
   - **Entities** card → добавьте слайдеры speed, scale, color, preset
   - **Entities** card → добавьте переключатели rotation, presence, work hours

### 5.2. Через YAML (полная панель)

1. **Settings** → **Dashboards** → **Add Dashboard**
2. Выберите имя, включите **"Take control of the YAML"**
3. Откройте дашборд → три точки → **Raw configuration editor**
4. Вставьте содержимое раздела `views:` из файла `ha_config.yaml`
5. Замените все `xxxx` на ID вашей лампы (в нижнем регистре!)

---

## Шаг 6: Проверка что всё работает

### Чек-лист:

- [ ] Home Assistant открывается по `http://<IP-NAS>:8123`
- [ ] MQTT интеграция подключена (зелёная галочка в Devices & Services)
- [ ] Лампа показывает **MQTT: Connected** на своей веб-странице
- [ ] Устройство **GyverLamp2 XXXX** появилось в HA (Settings → Devices)
- [ ] Можно включить/выключить лампу из HA
- [ ] Можно менять яркость и эффекты из HA
- [ ] Слайдеры speed/scale/color работают
- [ ] Автоматизация timezone sync создана и активна
- [ ] (Опционально) Присутствие работает — лампа реагирует на home/away

---

## Устранение проблем

### Лампа не появляется в HA
1. Проверьте что MQTT включён на лампе (cfg.mqtt = 1)
2. Проверьте IP брокера, порт, логин, пароль
3. Откройте Serial Monitor лампы — ищите строку `MQTT connecting... OK` или `FAIL`
4. В HA → **Settings** → **Devices & Services** → **MQTT** → **Configure** → **Listen to topic** → введите `homeassistant/#` → **Start listening** → проверьте приходят ли discovery-сообщения

### MQTT подключается и отключается
- Убедитесь что у каждого MQTT клиента уникальный ID
- Если у вас несколько ламп — каждая имеет уникальный ID из chip ID

### Присутствие не работает
1. Проверьте что `person.имя` корректно показывает `home`/`not_home` в HA
2. Проверьте что автоматизации включены (не disabled)
3. Убедитесь что переключатель **Presence** включён на лампе
4. Помните: лампа включается только в рабочие часы (work_from — work_to)

### Часовой пояс неправильный
1. Убедитесь что HA настроен на правильный часовой пояс (**Settings** → **General** → **Time Zone**)
2. Проверьте что автоматизация timezone sync включена
3. Проверьте переменную `TZ` в Docker контейнере HA

---

## Полезные ссылки

- Home Assistant: https://www.home-assistant.io/
- Mosquitto: https://mosquitto.org/
- MQTT интеграция HA: https://www.home-assistant.io/integrations/mqtt/
- HA Companion App: https://companion.home-assistant.io/
