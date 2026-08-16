#include <Arduino.h>

#include "audio/bt_audio.h"
#include "config/config_manager.h"
#include "input/button_mapper.h"
#include "setup/ap_setup.h"

namespace {
    constexpr uint8_t BOOT_BUTTON_PIN = 0; // onboard BOOT button
    // Onboard blue LED, see docs/hardware.md.
    constexpr uint8_t STATUS_LED_PIN = 2;
    constexpr unsigned long SETUP_WINDOW_MS = 5000;
    constexpr unsigned long LED_BLINK_INTERVAL_MS = 250;

    bool apModeActive = false;

    // Waits up to SETUP_WINDOW_MS for a BOOT press, blinking the onboard LED
    // as a visual cue, and returns true as soon as it's pressed. Deliberately
    // polled *after* the normal boot has already completed - GPIO0 is an
    // ESP32 boot-mode strapping pin, so holding it through the reset itself
    // makes the ROM enter UART download mode instead of running the firmware
    // at all (see docs/entwicklung.md).
    bool waitForSetupRequest() {
        pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
        pinMode(STATUS_LED_PIN, OUTPUT);

        unsigned long start = millis();
        unsigned long lastBlink = start;
        bool ledOn = false;

        while (millis() - start < SETUP_WINDOW_MS) {
            if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
                return true;
            }
            if (millis() - lastBlink >= LED_BLINK_INTERVAL_MS) {
                ledOn = !ledOn;
                digitalWrite(STATUS_LED_PIN, ledOn);
                lastBlink = millis();
            }
        }
        return false;
    }
}

void setup() {
    Serial.begin(115200);
    delay(200);

    ConfigManager::begin();

    bool setupRequested = false;
    if (ConfigManager::isConfigured()) {
        Serial.printf("[Main] Configured -> press BOOT within %lus for setup mode\n",
            SETUP_WINDOW_MS / 1000);
        setupRequested = waitForSetupRequest();
    }

    apModeActive = !ConfigManager::isConfigured() || setupRequested;

    // Solid on for the whole AP-setup session, off for the whole Bluetooth
    // session - lets you tell the mode apart at a glance.
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, apModeActive ? HIGH : LOW);

    if (apModeActive) {
        Serial.println(setupRequested
            ? "[Main] BOOT pressed -> starting AP setup mode"
            : "[Main] No saved setup -> starting AP setup mode");
        ApSetup::begin();
    } else {
        Serial.println("[Main] No BOOT press -> starting Bluetooth operating mode");
        BtAudio::begin();
        ButtonMapper::begin();
    }
}

void loop() {
    if (apModeActive) {
        ApSetup::loop();
    } else {
        BtAudio::loop();
        ButtonMapper::loop();
    }
}
