---
head:
  - - script
    - type: module
      src: https://unpkg.com/esp-web-tools@10.4.0/dist/web/install-button.js
---

# Firmware flashen

**Voraussetzungen:**
- Chrome, Edge oder Opera auf einem Desktop-Betriebssystem (Windows/macOS/Linux).
  Firefox und Safari unterstützen Web Serial nicht, mobile Browser generell nicht.
- Ein Daten-USB-Kabel (reine Ladekabel funktionieren nicht. Der ESP32 muss als
  serieller Port erkannt werden).
- Ggf. ein USB-Treiber für den seriellen Chip auf dem jeweiligen ELEGOO-Board
  (meist CP2102 oder CH340).

## Flashen über Browser

Drei Varianten, je nachdem was dabei erhalten bleiben soll:

### Komplette Neuinstallation

Löscht alles, also die alte Firmware auf dem ESP32, das Dateisystem sowie
die Einstellungen.

::: warning Warnung
Die Bluetooth-Box muss neu gekoppelt werden, MP3-Dateien werden gelöscht und
das AP-Setup muss erneut durchgeführt werden.
:::

<esp-web-install-button manifest="firmware/manifest-erase.json"></esp-web-install-button>

### Firmware Reset

Schreibt Firmware und Dateisystem neu. Die Bluetooth-Kopplung bleibt
erhalten. Im folgenden Dialog **"Erase device before installing" nicht
anhaken**, sonst geht auch die Konfiguration verloren (siehe "Komplette
Neuinstallation" oben).

::: warning Warnung
Der AP-Setup-Modus startet nicht automatisch, und die MP3-Dateien müssen neu
hochgeladen werden.
:::

<esp-web-install-button manifest="firmware/manifest-reset.json"></esp-web-install-button>

### Firmware-Update

Update der Core-Firmware. Die Logik zum Abspielen der Sounds wird auf die
neueste Version aktualisiert. Die Setup-Webseite bleibt auf dem alten Stand.
MP3-Dateien und Bluetooth-Konfiguration bleiben erhalten.

::: warning Warnung
Im folgenden Dialog **"Erase device before installing" nicht anhaken**.
:::

<esp-web-install-button manifest="firmware/manifest-update.json"></esp-web-install-button>

## Manuell per esptool flashen (Fortgeschrittene)

Ohne Browser-Unterstützung (z.B. Firefox/Safari) geht es auch klassisch mit
[esptool](https://docs.espressif.com/projects/esptool/en/latest/esp32/):

```bash
pip install esptool
```

Je nach System heißen die Befehle stattdessen `pip3`/`python3` statt
`pip`/`python` (v.a. unter macOS und Linux).

Die `.bin`-Dateien liegen als Assets am
[`latest`-Release](https://github.com/Roy0815/paintball-game-start-buzzer/releases/tag/latest)
zum Download bereit. In den Befehlen unten muss `<PORT>` durch den
tatsächlichen seriellen Port des ESP32 ersetzt werden (z.B. `COM5` unter
Windows, `/dev/ttyUSB0` unter Linux, `/dev/cu.usbserial-0001` unter macOS).
Wie man den findet, hängt vom Betriebssystem ab:

### Windows

Geräte-Manager öffnen (Rechtsklick auf Start → "Geräte-Manager") und unter
**"Anschlüsse (COM & LPT)"** nachsehen. Der ESP32 erscheint dort als
serielle Schnittstelle, z.B. `Silicon Labs CP210x USB to UART Bridge (COM5)`
oder `USB-SERIAL CH340 (COM5)`. Die COM-Nummer (hier `COM5`) ist der Port.

Taucht gar kein Eintrag auf: der USB-Treiber für den seriellen Chip fehlt
(siehe Voraussetzungen oben).

### macOS

Terminal öffnen und `ls /dev/cu.*` einmal vor und einmal nach dem Anstecken
des ESP32 ausführen. Der neu hinzugekommene Eintrag ist der gesuchte Port,
z.B. `/dev/cu.usbserial-0001` (CP2102) oder `/dev/cu.wchusbserial*` (CH340).

Für CH340-basierte Boards ist unter macOS oft ein separater Treiber nötig,
da Apple ihn nicht automatisch signiert mitliefert.

### Linux

Terminal öffnen und `ls /dev/ttyUSB*` (CP2102) bzw. `ls /dev/ttyACM*`
ausführen, oder direkt nach dem Anstecken `dmesg | tail` ausführen. Dort
taucht der neu erkannte Port auf (z.B. `/dev/ttyUSB0`). Beide Chips werden vom
Linux-Kernel direkt unterstützt, kein zusätzlicher Treiber nötig.

Bei `Permission denied`: der User muss in der Gruppe `dialout` sein
(`sudo usermod -aG dialout $USER`, danach neu einloggen bzw. WSL neu
starten). Unter WSL zusätzlich
[usbipd-win einrichten](/entwicklung#usb-passthrough-einrichten).

### Flash-Befehle

Bewusst als Einzeiler ohne Zeilenumbrüche. Ein abschließendes `\` für
Zeilenfortsetzung funktioniert nur in bash/zsh (macOS-Terminal, Linux, Git
Bash, WSL), nicht in der Windows-`cmd`/PowerShell. So lassen sich die
Befehle unter jedem Betriebssystem 1:1 kopieren.

Komplette Neuinstallation (löscht auch NVS/Konfiguration):

```bash
python -m esptool --chip esp32 --port <PORT> --baud 460800 erase_flash
python -m esptool --chip esp32 --port <PORT> --baud 460800 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x250000 littlefs.bin
```

Firmware reset (Konfiguration bleibt, MP3s gehen verloren):

```bash
python -m esptool --chip esp32 --port <PORT> --baud 460800 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x250000 littlefs.bin
```

Firmware-Update (Dateisystem und Konfiguration bleiben erhalten):

```bash
python -m esptool --chip esp32 --port <PORT> --baud 460800 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin
```

Bricht der Schreibvorgang mit `Failed to connect` oder mittendrin ab, hilft
oft ein niedrigerer Wert für `--baud` (z.B. `115200`).

## Alternative: PlatformIO/CLI

Wer lieber mit PlatformIO arbeitet (z.B. für eigene Firmware-Änderungen),
findet die CLI-basierte Variante unter [Entwicklung](/entwicklung).

## Woher kommt die Firmware hier?

Bei jedem Push auf `main`, der Firmware-relevante Dateien ändert, baut
[`build-firmware.yml`](https://github.com/Roy0815/paintball-game-start-buzzer/blob/main/.github/workflows/build-firmware.yml)
die Firmware neu und veröffentlicht sie als Assets des rollierenden
[`latest`-Release](https://github.com/Roy0815/paintball-game-start-buzzer/releases/tag/latest).
Diese Seite (gebaut von
[`docs-deploy.yml`](https://github.com/Roy0815/paintball-game-start-buzzer/blob/main/.github/workflows/docs-deploy.yml))
lädt sich die Assets von genau diesem Release, statt selbst neu zu bauen.
Die Firmware-Dateien werden nicht im Repository versioniert. Die angezeigte
Version entspricht dem kurzen Git-Commit-Hash, aus dem gebaut wurde.
