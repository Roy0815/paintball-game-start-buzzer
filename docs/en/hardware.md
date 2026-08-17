# Hardware & Wiring

## Components

- **ELEGOO ESP32-WROOM-32 DevKit** (ASIN B0D8T7LZF2), 4 MB flash
- **RF relay module** as the signal source for the button press (an RF
  remote triggers the relay). The button contact (COM/NO) is used as a
  **potential-free (dry) contact**. It doesn't carry any voltage of its
  own, it just closes a circuit on trigger, which the ESP32 detects via its
  internal pull-up. That makes the wiring independent of whatever voltage
  the relay module itself runs on internally.
- Bluetooth speaker/box with A2DP support (classic Bluetooth, not BLE)

## Parts List

### Shopping List

| Part                                            | Qty | Price       | Note                                                                                                                                                      | Link                                                                     |
| ----------------------------------------------- | --- | ----------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------ |
| ESP32-WROOM-32 DevKit (e.g. ELEGOO, 4 MB flash) | 1   | approx. €15 | ASIN B0D8T7LZF2 (price and link are for a 2-pack)                                                                                                         | [Amazon](https://amzn.to/4wRAqzT) \*                                     |
| RF relay module                                 | 1   | approx. €10 | Must run on **5V**. Many relay modules are built for a 12V coil. Check explicitly for 5V compatibility when buying.                                       | [Amazon](https://www.amazon.de/dp/B0DJX32J75?tag=paintball-buzzer-21) \* |
| Jumper wires, female-to-male                    | 4   | approx. €7  | For V+/V-/COM/NO between the relay module and the ESP32, see wiring below. Usually noticeably cheaper at a local electronics store than as an online set. | [Amazon](https://amzn.to/4xPisik) \*                                     |

> \* Affiliate Links. The items don't increase in price for you, but generate a few cents for me. The products can be bought on any platform, I just use amazon for convenience.

### Probably Already Have

| Part                            | Note                                                                                                                                  |
| ------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------- |
| Power bank, 5V output           | Powers the ESP32 (VIN) for mobile/battery-powered operation, and through it, the relay module too (see [Power Supply](#power-supply)) |
| USB-C cable                     | For flashing, and for power from the power bank                                                                                       |
| Bluetooth speaker, A2DP-capable | Classic Bluetooth, not BLE                                                                                                            |

### Case

3D-printed case for stable transport and easy operation:
_(Link coming)_

## Wiring (simplified)

Only the parts that actually need external wiring - the BOOT button and LED
already sit onboard the ESP32 board and don't need any wiring of their own
(details in the pin table below):

```mermaid
C4Container
    title Wiring: RF Relay Module and ESP32
    UpdateLayoutConfig($c4ShapeInRow="1", $c4BoundaryInRow="2")

    Container_Boundary(relais, "Relay") {
        Component(v+, "V+", "", "")
        Component(v-, "V-", "", "")
        Component(com, "COM", "", "")
        Component(no, "NO", "", "")
    }

    Container_Boundary(esp, "ESP32") {
        Component(vin, "VIN", "", "")
        Component(gnd, "GND", "", "")
        Component(gnd2, "GND", "", "")
        Component(gpio4, "GPIO4", "D4", "")
    }

    Rel(v+, vin, "supply", "")
    Rel(v-, gnd, "supply", "")
    Rel(com, gnd2, "button contact", "")
    Rel(no, gpio4, "button contact", "")
```

Each pin connection is its own, individually labeled relationship between
the two component boxes.

COM and NO belong to the same dry contact: when the remote triggers it, the
contact briefly bridges COM to NO. This pulls `GPIO4` (thanks to the
internal `INPUT_PULLUP`) low for the duration of the button press. The supply lines
(V+/V-) are a completely separate circuit powering the relay module
itself.

<div style="display: flex; gap: 1rem; flex-wrap: wrap;">
  <img src="/cable-connections-raw.jpg" alt="Wiring, outside the case" style="flex: 1 1 300px; max-width: 100%;">
  <img src="/assembled-in-box.jpg" alt="Assembled in the case" style="flex: 1 1 300px; max-width: 100%;">
</div>

<details>
<summary><strong>Advanced: Pin Table</strong></summary>

| ESP32 Pin   | Component                        | Function                                                                                                                                                                                                                                                           |
| ----------- | -------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `GPIO4`     | RF relay module, **NO** contact  | Signal input, button-press trigger, `INPUT_PULLUP`, active low                                                                                                                                                                                                     |
| `GND`       | RF relay module, **COM** contact | reference potential for the button contact                                                                                                                                                                                                                         |
| `VIN` (5 V) | RF relay module, **V+** (supply) | power supply for the relay module                                                                                                                                                                                                                                  |
| `GND`       | RF relay module, **V-** (supply) | common ground, required, see below                                                                                                                                                                                                                                 |
| `GPIO0`     | BOOT button (onboard)            | no external component, press within 5s **after** boot to enter setup mode (see [Development](/en/development#getting-back-into-setup-mode)), **don't** hold during reset/EN                                                                                        |
| `GPIO2`     | Onboard LED (blue)               | no external component, blinks during the 5s window, then stays solid on for the whole AP setup session, off during Bluetooth operation. Verified on the ELEGOO board (ASIN B0D8T7LZF2); the separate red LED is hardwired to the power supply and not controllable |

</details>

## Known Pitfalls

- **3.3 V, not 5 V-tolerant:** the ESP32's logic pins only tolerate 3.3 V
  continuously. A 5V signal wired directly into a GPIO can damage the chip.
  That doesn't apply to `GPIO4` in this wiring, since the dry contact
  carries no voltage of its own. It matters, though, if the relay module is later swapped
  for one with a direct digital signal output (e.g. a "DATA"/"VT" pin
  instead of a switch contact): check the datasheet for that output's
  voltage level first, and add a voltage divider or level shifter if it's
  not 3.3V-compatible.
- **Don't confuse VIN with the 3.3V pin:** `VIN` expects roughly 5V and gets
  regulated down to 3.3V internally. That's what powers the relay module
  here. The `3.3V` pin, by contrast, is already the board's regulated output
  voltage and must **not** be used as an input for an external 5V source.
- **Keep the relay module's supply voltage and signal level separate:** even
  though the module runs internally on 5V (common for these RF receivers),
  that only concerns the V+/V- supply line. The button contact (COM/NO)
  stays independently potential-free. That's exactly why this contact is
  used here instead of any direct signal output the module might also have.
- **Don't forget the common ground:** the relay module's GND and the ESP32's
  GND must be connected, even though both are powered from the same
  `VIN`/`GND` pin pair. Without a common ground, `GPIO4` won't read the
  contact state reliably.
- **`GPIO0` is a boot-strapping pin:** unlike `GPIO4`, `GPIO0` must not be
  low during an EN reset/power-on. The ESP32's ROM samples the level at
  exactly that moment and, if low, enters UART download mode
  (`waiting for download`) instead of starting the firmware. The BOOT button
  can therefore only be pressed to get back into setup mode **after** a boot
  has already completed, see
  [Development](/en/development#getting-back-into-setup-mode).

## Power Supply

Depending on where it's deployed, power comes either via USB (development/
testing) or an external 5V source (power bank) on VIN. The relay module
gets its 5V supply **from the ESP32**, whose `VIN` pin acts as the shared
connection point for V+/V- (see the wiring above), **not** directly from
USB or the power bank. From the project's Pi Zero W backstory (see
[Lessons Learned](/en/troubleshooting)), a weak/unstable power supply is
known to cause hard-to-diagnose glitches. Especially while an active
Bluetooth connection is running, make sure the supply is adequately
rated.
