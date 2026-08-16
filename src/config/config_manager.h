#pragma once

#include <Arduino.h>

// Max. number of mapping rows (= max. number of MP3s).
constexpr size_t MAX_MAPPING_ENTRIES = 16;

struct MappingEntry {
    String filename;
    unsigned long fromMs;
    long toMs; // -1 = no upper limit
};

// NVS access (Preferences) for all stored settings, see docs/architektur.md.
namespace ConfigManager {
    void begin();

    bool isConfigured();
    void setConfigured(bool value);

    String getBluetoothName();
    void setBluetoothName(const String& name);

    String getBluetoothMac();
    void setBluetoothMac(const String& mac);

    String getMappingJson();
    void setMappingJson(const String& json);

    // Sorted ascending by fromMs.
    size_t getMapping(MappingEntry* outEntries, size_t maxEntries);
}
