# Lessons Learned

Design-Entscheidungen, Limitierungen und offene Baustellen, die sich nicht
von selbst aus dem Code erschließen.

## Pi Zero W → ESP32

- Ursprünglich ein Raspberry-Pi-Projekt (RF-Relais löst MP3 über Bluetooth
  aus), lief stabil auf einem Pi 3B+.
- Zielhardware Pi Zero W gescheitert: geteilte WLAN/Bluetooth-Antenne führte
  zu ~6 s A2DP-Latenz, dazu schwaches WLAN-Signal und Stromversorgungsprobleme.
- Der ESP32 hat dieselbe geteilte Antenne. Deshalb strikt getrennt: WLAN nur
  im Setup-Modus, Bluetooth nur im Betriebsmodus, nie beides gleichzeitig
  (siehe [Architektur](/architektur)).

## MP3-Decoding: arduino-audio-tools statt ESP8266Audio

- `ESP8266Audio` scheitert an einem offenen, ungelösten Bug
  ([Issue #155](https://github.com/earlephilhower/ESP8266Audio/issues/155)).
- Ersetzt durch `arduino-audio-tools` (`EncodedAudioStream` +
  `MP3DecoderHelix`), die von pschatzmann (Autor von `ESP32-A2DP`) selbst
  empfohlene Kombination, dekodiert direkt beim Lesen aus LittleFS.
- `decoder.resizeReadResultQueue(8192)` nötig, sonst gelegentlich
  `"Could not write result to out"` bei Funkstörungen.

## Kein OTA

- Partitionstabelle hat nur einen App-Slot statt `ota_0`/`ota_1`. Flashen
  läuft ohnehin über USB oder Browser, OTA bringt hier nichts.
- Dafür mehr Platz für LittleFS (~1,69 statt ~1,3 MB bei Standard-Layout).

## Partitions-SubType: `spiffs`, nicht `littlefs`

- Der Partitionstabellen-Generator kennt `littlefs` nicht als SubType-Keyword
  (nur `spiffs`, `ota`, `nvs`, ...). `pio run` bricht sonst mit "Value
  'littlefs' is not valid" ab.
- Die Name-Spalte (`littlefs`) ist trotzdem korrekt und wird als
  `partitionLabel` in `LittleFS.begin(true, "/littlefs", 10, "littlefs")`
  verwendet. Default der Bibliothek wäre sonst `"spiffs"` und würde die
  falsche Partition suchen.

## 1,5-MB-MP3-Limit

- Hart geprüft, client- und serverseitig (`MAX_TOTAL_MP3_BYTES` in
  `ap_setup.cpp`).

## Offen: `bt_audio.cpp` ungetestet auf Hardware

- `set_auto_reconnect(esp_bd_addr_t)` und `set_ssid_callback` sind aus der
  `ESP32-A2DP`-Doku abgeleitet, nicht live verifiziert.
- Beim ersten Testlauf per Serial Monitor (VS Code: PlatformIO → Monitor,
  oder `pio device monitor`) beobachten, ob MAC-Erfassung und Fast-Reconnect
  wie erwartet laufen.

## Troubleshooting

- **Browser-Flasher findet kein Gerät**: nur Chrome, Edge oder Opera auf
  Desktop. Firefox, Safari und mobile Browser können kein Web Serial.
- **Kurze Tastendrücke werden nicht erkannt**: `DEBOUNCE_MS` in
  `src/input/relay_control.cpp` anpassen (Default 50 ms).
- **Upload schlägt fehl**: siehe 1,5-MB-Limit oben.
- **Serial Monitor** (VS Code: PlatformIO → Monitor, oder
  `pio device monitor`) ist der wichtigste Debug-Kanal für Verbindungsstatus
  und Wiedergabe-Events.
