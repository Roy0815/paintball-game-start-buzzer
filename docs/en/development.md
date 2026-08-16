# Development

## Project structure

```
/platformio.ini          PlatformIO configuration (board, libraries, partitions)
/partitions_custom.csv   Partition table (app + LittleFS)
/src                      Firmware source code
/data                     Files for the LittleFS filesystem (setup.html)
/docs                     This VitePress documentation
```

## Installing PlatformIO

Recommended: install the
[PlatformIO IDE extension for VS Code](https://platformio.org/install/ide?install=vscode).
It bundles PlatformIO Core (the `pio` CLI) automatically. No separate
`pip install` step is needed.

> **PATH gotcha:** The extension installs `pio` into its own isolated venv
> (`~/.platformio/penv/bin`), which is **not** on the system PATH. Called
> directly from a regular terminal (`pio ...` or `npm run ...`), `pio`
> therefore reports "not found", even though the extension itself works
> fine. This doesn't matter for working through the VS Code UI (see below);
> the extension invokes its own `pio` copy internally. If you also need
> `pio` in a plain terminal (e.g. for your own scripts), you have two
> options:
> - add `~/.platformio/penv/bin` to your shell config (`~/.bashrc` or
>   similar), or
> - additionally run `pip install --upgrade platformio`, which lands
>   directly in a PATH directory like `~/.local/bin`.

## Flashing from VS Code

With the PlatformIO extension installed:

- **PlatformIO icon** (alien head) in the left Activity Bar → **Project
  Tasks** → `esp32dev` → **General** → `Upload`, `Upload Filesystem Image`,
  or `Monitor`
- or the toolbar at the bottom left of the VS Code window (checkmark =
  Build, arrow = Upload, plug = Monitor). This appears automatically once
  `platformio.ini` is in the workspace

This is equivalent to `pio run -t upload`, `pio run -t uploadfs`, or
`pio device monitor`, just without needing `pio` on the terminal PATH.
Under WSL, **USB passthrough** is still required either way (see below).
That's independent of whether you use the CLI or VS Code, and always has to
be done manually.

## PlatformIO workflow (CLI)

Alternatively, or for CI/scripts, use the `pio` CLI directly (see the PATH
setup note above):

```bash
pio run              # compile
pio run -t upload    # flash the firmware
pio run -t uploadfs  # flash the LittleFS data (data/)
pio device monitor   # open the serial monitor
```

## Developing under WSL

Development happens in WSL2, and the PlatformIO build runs natively there.
**Flashing** requires USB passthrough (`usbipd-win`). This step is manual
and has to be repeated every time the ESP32 is plugged back in.

> If you just want to get the finished firmware onto a board (without
> making your own code changes), you can skip the whole usbipd topic:
> see [Flash the firmware](/en/flash). Development
> itself (changing code, compiling) still needs PlatformIO under WSL.

### Setting up USB passthrough

1. Install [`usbipd-win`](https://github.com/dorssel/usbipd-win) on Windows
2. Plug in the ESP32, find the device with `usbipd list` (PowerShell)
3. `usbipd bind --busid <ID>` (once, as Administrator)
4. `usbipd attach --wsl --busid <ID>` (**every time** it's plugged back in)
5. The device shows up in WSL as `/dev/ttyUSB0` (or similar)

### Multiple serial devices

If several devices are plugged in at once, automatic port detection can
fail. Set the port explicitly:

- **VS Code:** the PlatformIO icon → **Devices** shows the detected ports;
  pick the one you want under **Project Tasks → esp32dev → Advanced →
  Upload Port / Monitor Port**.
- **CLI:** `pio run -t upload --upload-port /dev/ttyUSB0` or
  `pio device monitor --port /dev/ttyUSB0`

## Getting back into setup mode

To change the speaker name, MP3s, or mapping after the initial setup
**without reflashing**: reset the board (press EN briefly, or unplug/replug
power) and **then**, within 5 seconds, press the BOOT button (GPIO0). The
onboard LED blinks during that window. If `src/main.cpp` detects the press,
the ESP32 boots into AP setup mode regardless of the saved `configured`
flag, otherwise it starts normally in Bluetooth operating mode. Afterward
the LED stays on as a mode indicator (solid on for AP setup, off for
Bluetooth operation).

> **Important:** do **not** hold BOOT while pressing EN/powering on. `GPIO0`
> is one of the ESP32's boot-strapping pins. If it's low at the exact
> moment the reset ends, the ROM bootloader enters UART download mode
> (`waiting for download` in the serial monitor) instead of starting the
> firmware. The firmware (and with it the BOOT check) then never runs at
> all. That's exactly the classic manual key combo for putting the ESP32
> into flashing mode (see [Flash the firmware](/en/flash)
> / "Flashing manually with esptool"). If the ESP32 gets stuck at
> `waiting for download`, just reset it again without touching BOOT this
> time.

## Viewing the docs locally

```bash
npm run docs:dev
```

## Building the docs (same as the GitHub Actions workflow)

```bash
npm run docs:build
```
