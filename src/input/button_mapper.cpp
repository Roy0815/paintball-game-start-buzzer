#include "button_mapper.h"

#include <Arduino.h>

#include "../audio/bt_audio.h"
#include "../config/config_manager.h"
#include "relay_control.h"

namespace {
    bool wasActive = false;
    unsigned long pressStart = 0;
}

void ButtonMapper::begin() {
    RelayControl::begin();
}

void ButtonMapper::loop() {
    RelayControl::update();
    bool active = RelayControl::isActive();

    if (active && !wasActive) {
        pressStart = millis();
    }

    if (!active && wasActive) {
        unsigned long pressDuration = millis() - pressStart;

        if (BtAudio::isPlaying()) {
            // The next button press stops an ongoing playback immediately.
            BtAudio::stop();
        } else {
            MappingEntry entries[MAX_MAPPING_ENTRIES];
            size_t count = ConfigManager::getMapping(entries, MAX_MAPPING_ENTRIES);

            for (size_t i = 0; i < count; i++) {
                bool aboveFrom = pressDuration >= entries[i].fromMs;
                bool belowTo = (entries[i].toMs < 0) || (pressDuration <= (unsigned long)entries[i].toMs);
                if (aboveFrom && belowTo) {
                    String path = "/mp3/" + entries[i].filename;
                    BtAudio::play(path.c_str());
                    break;
                }
            }
        }
    }

    wasActive = active;
}
