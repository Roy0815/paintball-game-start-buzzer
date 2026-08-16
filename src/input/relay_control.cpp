#include "relay_control.h"

#include <Arduino.h>

namespace {
    constexpr uint8_t RELAY_PIN = 4;
    constexpr unsigned long DEBOUNCE_MS = 50;

    bool lastRawState = false;
    unsigned long lastRawChangeTime = 0;
    bool debouncedState = false;
}

void RelayControl::begin() {
    pinMode(RELAY_PIN, INPUT_PULLUP);
}

void RelayControl::update() {
    bool rawState = (digitalRead(RELAY_PIN) == LOW);

    // Raw signal changes -> reset the debounce timer.
    if (rawState != lastRawState) {
        lastRawChangeTime = millis();
        lastRawState = rawState;
    }

    // Only adopt as "real" once the raw signal has been stable for DEBOUNCE_MS.
    if ((millis() - lastRawChangeTime) > DEBOUNCE_MS) {
        debouncedState = rawState;
    }
}

bool RelayControl::isActive() {
    return debouncedState;
}
