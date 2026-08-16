# Entwicklung

## Projektstruktur

```
/platformio.ini          PlatformIO-Konfiguration (Board, Libraries, Partitionen)
/partitions_custom.csv   Partitionstabelle (App + LittleFS)
/src                      Firmware-Quellcode
/data                     Dateien fürs LittleFS-Dateisystem (setup.html)
/docs                     Diese VitePress-Dokumentation
```

## PlatformIO installieren

Empfohlen: die
[PlatformIO IDE-Extension für VS Code](https://platformio.org/install/ide?install=vscode)
installieren. Sie bringt PlatformIO Core (die `pio`-CLI) automatisch mit.
Ein separater `pip install`-Schritt ist nicht nötig.

> **Achtung PATH:** Die Extension installiert `pio` in ein eigenes,
> isoliertes venv (`~/.platformio/penv/bin`), das **nicht** im System-PATH
> liegt. Direkt aus einem normalen Terminal aufgerufen (`pio ...` oder
> `npm run ...`), meldet sich `pio` deshalb mit "not found", auch wenn die
> Extension selbst einwandfrei funktioniert. Für die Arbeit über die
> VS-Code-Oberfläche (siehe unten) spielt das keine Rolle, die Extension
> ruft ihre eigene `pio`-Kopie intern auf. Wer `pio` zusätzlich in einem
> normalen Terminal braucht (z.B. für eigene Skripte), hat zwei Optionen:
> - `~/.platformio/penv/bin` zur Shell-Konfiguration (`~/.bashrc` o.ä.)
>   hinzufügen, oder
> - zusätzlich `pip install --upgrade platformio` ausführen, das landet
>   direkt in einem PATH-Ordner wie `~/.local/bin`.

## Flashen über VS Code

Mit installierter PlatformIO-Extension:

- **PlatformIO-Icon** (Alien-Kopf) in der Activity Bar links → **Project
  Tasks** → `esp32dev` → **General** → `Upload`, `Upload Filesystem Image`
  oder `Monitor`
- oder die Symbolleiste unten links im VS-Code-Fenster (Haken = Build,
  Pfeil = Upload, Stecker = Monitor). Die Symbolleiste erscheint automatisch,
  sobald `platformio.ini` im Workspace liegt

Das entspricht `pio run -t upload`, `pio run -t uploadfs` bzw.
`pio device monitor`, nur ohne dass `pio` im Terminal-PATH liegen muss.
Unter WSL bleibt **USB-Passthrough** trotzdem Voraussetzung (siehe unten).
Das ist unabhängig vom gewählten Weg (CLI oder VS Code) und muss immer
manuell erledigt werden.

## PlatformIO-Workflow (CLI)

Alternativ bzw. für CI/Skripte direkt über die `pio`-CLI (siehe oben zum
PATH-Setup):

```bash
pio run              # kompilieren
pio run -t upload    # Firmware flashen
pio run -t uploadfs  # LittleFS-Daten (data/) flashen
pio device monitor   # seriellen Monitor öffnen
```

## Entwicklung unter WSL

Entwicklung erfolgt in WSL2, der PlatformIO-Build läuft dort nativ. Für das
**Flashen** ist USB-Passthrough nötig (`usbipd-win`). Dieser Schritt ist
manuell und muss nach jedem Neu-Anstecken des ESP32 wiederholt werden.

> Wer nur die fertige Firmware auf ein Board bringen will (ohne eigene
> Code-Änderungen), kann das komplette usbipd-Thema umgehen: siehe
> [Firmware flashen](/flash). Für die Entwicklung selbst (Code
> ändern, kompilieren) bleibt PlatformIO unter WSL trotzdem nötig.

### USB-Passthrough einrichten

1. [`usbipd-win`](https://github.com/dorssel/usbipd-win) auf Windows installieren
2. ESP32 anschließen, Gerät mit `usbipd list` (PowerShell) finden
3. `usbipd bind --busid <ID>` (einmalig, als Administrator)
4. `usbipd attach --wsl --busid <ID>` (bei **jedem** Neu-Anstecken)
5. Gerät erscheint in WSL als `/dev/ttyUSB0` (o.ä.)

### Mehrere serielle Geräte

Sind mehrere Geräte gleichzeitig angeschlossen, kann die automatische
Port-Erkennung fehlschlagen. Port explizit vorgeben:

- **VS Code:** PlatformIO-Icon → **Devices** zeigt die erkannten Ports; über
  **Project Tasks → esp32dev → Advanced → Upload Port / Monitor Port** den
  gewünschten Port auswählen.
- **CLI:** `pio run -t upload --upload-port /dev/ttyUSB0` bzw.
  `pio device monitor --port /dev/ttyUSB0`

## Rückkehr in den Setup-Modus

Um nach der Ersteinrichtung Boxname, MP3s oder Mapping zu ändern, **ohne neu
zu flashen**: Board resetten (EN-Taster kurz drücken oder Strom
trennen/wieder anschließen) und **danach** innerhalb von 5 Sekunden den
BOOT-Taster (GPIO0) drücken. Die Onboard-LED blinkt während dieses
Zeitfensters. Erkennt `src/main.cpp` den Tastendruck, startet der ESP32 im
AP-Setup-Modus, unabhängig vom gespeicherten `configured`-Flag, sonst läuft
er normal im Bluetooth-Betrieb. Die LED bleibt danach als Modus-Indikator durchgehend
an (AP-Setup) bzw. aus (Bluetooth-Betrieb).

> **Wichtig:** BOOT **nicht** gedrückt halten und dabei EN drücken/den
> ESP32 einschalten. `GPIO0` ist einer der Boot-Strap-Pins des ESP32. Liegt
> er in dem Moment, in dem der Reset endet, auf Low, wählt der ROM-Bootloader
> den UART-Download-Modus (`waiting for download` im Serial Monitor) statt
> die Firmware zu starten. Die Firmware (und damit auch die
> BOOT-Erkennung) läuft dann gar nicht erst an. Genau das ist die klassische
> Handkombination, um den ESP32 manuell in den Flash-Modus zu versetzen (siehe
> [Firmware flashen](/flash) bzw. "Manuell per esptool flashen").
> Falls der ESP32 mit `waiting for download` hängen bleibt: einfach nochmal
> resetten, diesmal ohne BOOT zu berühren.

## Doku lokal ansehen

```bash
npm run docs:dev
```

## Doku-Build (wie im GitHub-Actions-Workflow)

```bash
npm run docs:build
```
