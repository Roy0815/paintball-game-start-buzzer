# First-Time Setup

This guide covers the initial setup via AP setup mode
([Mode A](/en/architecture#mode-a-ap-setup)). This mode is automatically
active as long as no setup has been saved yet (first boot after flashing).

## 1. Flash firmware and filesystem

The easiest way is straight from the browser:
[Flash the firmware](/en/flash)

On the very first flash, no setup is saved yet. The ESP32 will
automatically boot into AP setup mode afterwards.

## 2. Connect to the setup WiFi

The ESP32 opens an open (unencrypted) WiFi network named
**`Paintball-Setup`**. Connect to it with a phone or laptop.

## 3. Open the setup page

In a browser, go to `http://192.168.4.1/`.

## 4. Enter the Bluetooth speaker name

Enter the speaker's name exactly as it appears during normal Bluetooth
pairing (case-sensitive).

## 5. Upload MP3 files

Select one or more MP3 files and upload them via "Upload selected files".
The total size of all files is capped at **1.5 MB**. The page shows the
currently used size and rejects uploads that would exceed the limit.

The mapping table (step 6) shows each file's size. The ✕ button on the
right of each row deletes an already-uploaded file.

::: warning
Deleted files can't be recovered.
:::

## 6. Fill in the button-press mapping

Depending on how long the button on the relay is pressed, different MP3s
can be played.

A row is automatically added to the mapping table for each uploaded file.
Only the **"To (ms)"** column needs to be filled in. The "From" value of
the next row is calculated automatically (`from = previous row's to + 1`).
The last row doesn't need a "To" value; it covers everything above it.

Example with 3 files:

| File | From (ms) | To (ms) |
|---|---|---|
| audio1.mp3 | 0 | 500 |
| audio2.mp3 | 501 | 1500 |
| audio3.mp3 | 1501 | unlimited |

With only one uploaded file, there's just a single unbounded row. Every
button press triggers it.

The arrow buttons (▲/▼) on the right of each row change the order of the
files. The from/to boundaries stay bound to their ascending position. Moving
a file doesn't carry its time range along, the file instead takes over
whatever range its new position has. Any "To" value already entered stays
attached to that file, so double-check it after reordering.

## 7. Save

Click **"Save & Reboot"**. The ESP32 writes the speaker name, MP3 mapping,
and the `configured` flag to NVS storage and reboots, straight into
[Bluetooth operating mode](/en/architecture#mode-b-bluetooth-operation)
afterwards.

## 8. Verify the Bluetooth connection

The Bluetooth speaker needs to be in pairing mode on the first connection
and after software resets, so the ESP32 can connect to it. The connection
should then be established automatically.

On every later start of the ESP32, it's usually enough for the speaker to
just be turned on. If it doesn't work for a particular speaker, try pairing
mode again.

::: details Getting back into setup mode (later)

To later change the speaker name, MP3s, or mapping: restart the board via
the **EN** button (**"RESTART"** on the 3D-printed case) or by
interrupting the power supply. A blue light on the underside then blinks
for 5 seconds. While this light is blinking, press the **BOOT** button
(**"SETUP"** on the 3D-printed case). The LED should then turn solid blue,
and setup mode is active again. Details:
[Development → Getting back into setup mode](/en/development#getting-back-into-setup-mode).

:::
