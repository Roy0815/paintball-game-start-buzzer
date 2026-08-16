#include "config_manager.h"

#include <ArduinoJson.h>
#include <Preferences.h>

namespace {
    Preferences prefs;
    constexpr const char* NVS_NAMESPACE = "buzzer";
}

void ConfigManager::begin() {
    prefs.begin(NVS_NAMESPACE, false);
}

bool ConfigManager::isConfigured() {
    return prefs.getBool("configured", false);
}

void ConfigManager::setConfigured(bool value) {
    prefs.putBool("configured", value);
}

String ConfigManager::getBluetoothName() {
    return prefs.getString("bt_name", "");
}

void ConfigManager::setBluetoothName(const String& name) {
    prefs.putString("bt_name", name);
}

String ConfigManager::getBluetoothMac() {
    return prefs.getString("bt_mac", "");
}

void ConfigManager::setBluetoothMac(const String& mac) {
    prefs.putString("bt_mac", mac);
}

String ConfigManager::getMappingJson() {
    return prefs.getString("mapping", "{}");
}

void ConfigManager::setMappingJson(const String& json) {
    prefs.putString("mapping", json);
}

size_t ConfigManager::getMapping(MappingEntry* outEntries, size_t maxEntries) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, getMappingJson());
    if (err) {
        Serial.printf("[ConfigManager] Mapping JSON invalid: %s\n", err.c_str());
        return 0;
    }

    size_t count = 0;
    for (JsonPair kv : doc.as<JsonObject>()) {
        if (count >= maxEntries) break;
        JsonArray range = kv.value().as<JsonArray>();
        if (range.size() != 2) continue;

        outEntries[count].filename = kv.key().c_str();
        outEntries[count].fromMs = range[0].as<unsigned long>();
        outEntries[count].toMs = range[1].isNull() ? -1 : range[1].as<long>();
        count++;
    }

    // Insertion sort - the row count is small enough that anything fancier isn't worth it.
    for (size_t i = 1; i < count; i++) {
        MappingEntry key = outEntries[i];
        long j = (long)i - 1;
        while (j >= 0 && outEntries[j].fromMs > key.fromMs) {
            outEntries[j + 1] = outEntries[j];
            j--;
        }
        outEntries[j + 1] = key;
    }

    return count;
}
