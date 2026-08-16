# Lessons Learned

Design decisions, limitations, and open items that aren't obvious from the
code on its own.

## Pi Zero W → ESP32

- Originally a Raspberry Pi project (RF relay triggers MP3 over Bluetooth),
  ran stably on a Pi 3B+.
- Target hardware Pi Zero W failed: a shared WiFi/Bluetooth antenna caused
  ~6s of A2DP latency, plus a weak WiFi signal and power-supply issues.
- The ESP32 has the same shared antenna. Hence the strict separation: WiFi
  only in setup mode, Bluetooth only in operating mode, never both at once
  (see [Architecture](/en/architecture)).

## MP3 decoding: arduino-audio-tools instead of ESP8266Audio

- `ESP8266Audio` runs into an open, unresolved bug
  ([issue #155](https://github.com/earlephilhower/ESP8266Audio/issues/155)).
- Replaced with `arduino-audio-tools` (`EncodedAudioStream` +
  `MP3DecoderHelix`), the combination pschatzmann (author of `ESP32-A2DP`)
  himself recommends, decodes directly while reading from LittleFS.
- `decoder.resizeReadResultQueue(8192)` is needed, otherwise occasional
  `"Could not write result to out"` under radio interference.

## No OTA

- The partition table has a single app slot instead of `ota_0`/`ota_1`.
  Flashing happens over USB or the browser anyway, OTA buys nothing here.
- Frees up more room for LittleFS (~1.69 MB instead of ~1.3 MB with a
  standard layout).

## Partition SubType: `spiffs`, not `littlefs`

- The partition table generator doesn't recognize `littlefs` as a SubType
  keyword (only `spiffs`, `ota`, `nvs`, ...). `pio run` otherwise aborts
  with "Value 'littlefs' is not valid".
- The Name column (`littlefs`) is still correct and gets used as the
  `partitionLabel` in `LittleFS.begin(true, "/littlefs", 10, "littlefs")`.
  The library's default would otherwise be `"spiffs"` and look for the
  wrong partition.

## 1.5 MB MP3 limit

- Enforced both client- and server-side (`MAX_TOTAL_MP3_BYTES` in
  `ap_setup.cpp`).

## Open: `bt_audio.cpp` untested on hardware

- `set_auto_reconnect(esp_bd_addr_t)` and `set_ssid_callback` are inferred
  from the `ESP32-A2DP` docs, not verified live.
- Watch the serial monitor (VS Code: PlatformIO → Monitor, or
  `pio device monitor`) on the first real test run to confirm MAC capture
  and fast-reconnect behave as expected.

## Troubleshooting

- **Browser flasher finds no device**: only Chrome, Edge, or Opera on
  desktop. Firefox, Safari, and mobile browsers don't support Web Serial.
- **Short button presses aren't detected**: adjust `DEBOUNCE_MS` in
  `src/input/relay_control.cpp` (default 50 ms).
- **Upload fails**: see the 1.5 MB limit above.
- **Serial monitor** (VS Code: PlatformIO → Monitor, or
  `pio device monitor`) is the main debugging channel for connection status
  and playback events.
