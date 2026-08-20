---
head:
  - - script
    - type: module
      src: https://unpkg.com/esp-web-tools@10.4.0/dist/web/install-button.js
---

# Flash the Firmware

**Requirements:**
- Chrome, Edge, or Opera on a desktop OS (Windows/macOS/Linux). Firefox and
  Safari don't support Web Serial, and neither do mobile browsers in general.
- A USB data cable (charge-only cables won't work. The ESP32 needs to show
  up as a serial port).
- Possibly a USB driver for the serial chip on your particular ELEGOO board
  (usually a CP2102 or CH340).

## Flash via Browser

Three variants, depending on what should survive the process:

### Complete Fresh Install

Erases everything, meaning the old firmware on the ESP32, the filesystem,
and the settings.

::: warning
The Bluetooth speaker needs to be re-paired, MP3 files get deleted, and AP
setup needs to be done again.
:::

<esp-web-install-button manifest="../firmware/manifest-erase.json"></esp-web-install-button>

:::: details Other Browser Variants (Reset / Update)

### Firmware Reset

Writes firmware and filesystem fresh. The Bluetooth pairing is preserved. In
the dialog that follows, **don't check "Erase device before installing"**,
or the configuration is lost too (see "Complete Fresh Install" above).

::: warning
AP setup mode doesn't start automatically, and the MP3 files need to be
re-uploaded.
:::

<esp-web-install-button manifest="../firmware/manifest-reset.json"></esp-web-install-button>

### Firmware Update

Updates the core firmware. The sound-playback logic gets updated to the
latest version. The setup website stays at its old version. MP3 files and
the Bluetooth configuration are preserved.

::: warning
In the dialog that follows, **don't check "Erase device before
installing"**.
:::

<esp-web-install-button manifest="../firmware/manifest-update.json"></esp-web-install-button>

::::

::: details Flashing manually with esptool (Advanced)

Without browser support (e.g. Firefox/Safari), the classic route also
works, using [esptool](https://docs.espressif.com/projects/esptool/en/latest/esp32/):

```bash
pip install esptool
```

Depending on your system, the commands might be `pip3`/`python3` instead of
`pip`/`python` (mainly on macOS and Linux).

Download the following files from the assets of the
[`latest` release](https://github.com/Roy0815/paintball-game-start-buzzer/releases/tag/latest)
and put them in the same folder:
- `bootloader.bin`, `partitions.bin`, `firmware.bin` – needed for all three variants
- `littlefs.bin` – additionally needed only for "Complete Fresh Install" and
  "Firmware Reset" (not needed for "Firmware Update")

In the commands below, `<PORT>` needs to be replaced with the ESP32's
actual serial port (e.g. `COM5` on Windows, `/dev/ttyUSB0` on Linux,
`/dev/cu.usbserial-0001` on macOS). How to find it depends on your OS:

### Windows

Open Device Manager (right-click Start → "Device Manager") and look under
**"Ports (COM & LPT)"**. The ESP32 shows up there as a serial port, e.g.
`Silicon Labs CP210x USB to UART Bridge (COM5)` or
`USB-SERIAL CH340 (COM5)`. The COM number (here `COM5`) is the port.

Nothing shows up at all: the USB driver for the serial chip is missing
(see Requirements above).

### macOS

Open Terminal and run `ls /dev/cu.*` once before and once after plugging in
the ESP32. The newly added entry is the port you're looking for, e.g.
`/dev/cu.usbserial-0001` (CP2102) or `/dev/cu.wchusbserial*` (CH340).

CH340-based boards often need a separate driver on macOS, since Apple
doesn't ship a signed one automatically.

### Linux

Open a terminal and run `ls /dev/ttyUSB*` (CP2102) or `ls /dev/ttyACM*`, or
run `dmesg | tail` right after plugging in. The newly detected port shows
up there (e.g. `/dev/ttyUSB0`). Both chips are supported directly by the
Linux kernel, no extra driver needed.

Getting `Permission denied`: the user needs to be in the `dialout` group
(`sudo usermod -aG dialout $USER`, then log in again or restart WSL). Under
WSL, also
[set up usbipd-win](/en/development#setting-up-usb-passthrough).

### Flash Commands

Complete fresh install (also erases NVS/configuration):

```bash
python -m esptool --chip esp32 --port <PORT> --baud 460800 erase_flash
python -m esptool --chip esp32 --port <PORT> --baud 460800 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x250000 littlefs.bin
```

Firmware reset (configuration is preserved, MP3s are lost):

```bash
python -m esptool --chip esp32 --port <PORT> --baud 460800 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x250000 littlefs.bin
```

Firmware update (filesystem and configuration are preserved):

```bash
python -m esptool --chip esp32 --port <PORT> --baud 460800 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin
```

If the write aborts with `Failed to connect` or partway through, a lower
`--baud` value (e.g. `115200`) often helps.

:::

::: details Alternative: PlatformIO/CLI

If you'd rather work with PlatformIO (e.g. for your own firmware changes),
the CLI-based approach is described under [Development](/en/development).

:::

::: details Technical Background: Firmware Origin

Whenever a push to `main` changes firmware-relevant files,
[`build-firmware.yml`](https://github.com/Roy0815/paintball-game-start-buzzer/blob/main/.github/workflows/build-firmware.yml)
rebuilds the firmware and publishes it as assets on the rolling
[`latest` release](https://github.com/Roy0815/paintball-game-start-buzzer/releases/tag/latest).
This page (built by
[`docs-deploy.yml`](https://github.com/Roy0815/paintball-game-start-buzzer/blob/main/.github/workflows/docs-deploy.yml))
fetches its assets from that same release instead of building them itself.
The firmware files aren't checked into the repository. The version shown
corresponds to the short git commit hash it was built from.

:::
