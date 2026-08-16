#include "bt_audio.h"

#include <AudioTools.h>
#include <AudioTools/AudioCodecs/CodecMP3Helix.h>
#include <BluetoothA2DPSource.h>
#include <LittleFS.h>

#include "../config/config_manager.h"

namespace {
    BluetoothA2DPSource a2dp_source;

    File currentFile;
    MP3DecoderHelix mp3Decoder;
    EncodedAudioStream decoder(&currentFile, &mp3Decoder);

    bool playing = false;
    bool connected = false;

    // The A2DP stack pulls from this; the stream must never be empty, so
    // silence is used as a fallback.
    int32_t audioDataCallback(uint8_t* data, int32_t byteCount) {
        if (!playing) {
            memset(data, 0, byteCount);
            return byteCount;
        }

        int32_t bytesRead = decoder.readBytes(data, byteCount);
        if (bytesRead < byteCount) {
            memset(data + bytesRead, 0, byteCount - bytesRead);
            playing = false;
            currentFile.close();
            Serial.println("[BtAudio] Playback finished (end of file)");
        }
        return byteCount;
    }

    void onConnectionStateChanged(esp_a2d_connection_state_t state, void* /*obj*/) {
        connected = (state == ESP_A2D_CONNECTION_STATE_CONNECTED);
        switch (state) {
            case ESP_A2D_CONNECTION_STATE_CONNECTED:
                Serial.println("[BtAudio] A2DP connected");
                break;
            case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
                Serial.println("[BtAudio] A2DP disconnected");
                break;
            default:
                break;
        }
    }

    // Discovery callback: filters for the stored speaker name and remembers
    // its MAC address for future set_auto_reconnect() boots.
    bool onSsidDiscovered(const char* ssid, esp_bd_addr_t address, int /*rssi*/) {
        String storedName = ConfigManager::getBluetoothName();
        if (storedName.length() == 0 || storedName != ssid) {
            return false;
        }

        if (ConfigManager::getBluetoothMac().length() == 0) {
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                      address[0], address[1], address[2], address[3], address[4], address[5]);
            ConfigManager::setBluetoothMac(String(macStr));
            Serial.printf("[BtAudio] Speaker MAC address saved: %s\n", macStr);
        }
        return true;
    }

    bool parseMac(const String& macStr, esp_bd_addr_t out) {
        if (macStr.length() != 17) return false;
        unsigned int values[6];
        if (sscanf(macStr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
                    &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) {
            return false;
        }
        for (int i = 0; i < 6; i++) out[i] = (uint8_t)values[i];
        return true;
    }
}

void BtAudio::begin() {
    // partitionLabel, see docs/troubleshooting.md (spiffs-vs-littlefs pitfall).
    if (!LittleFS.begin(true, "/littlefs", 10, "littlefs")) {
        Serial.println("[BtAudio] ERROR: LittleFS could not be started");
    }

    a2dp_source.set_data_callback(audioDataCallback);
    a2dp_source.set_on_connection_state_changed(onConnectionStateChanged);
    a2dp_source.set_ssid_callback(onSsidDiscovered);
    // Prevents "Could not write result to out" during radio interference.
    decoder.resizeReadResultQueue(8192);

    // Reconnect behavior untested on hardware, see docs/troubleshooting.md.
    esp_bd_addr_t addr;
    String storedMac = ConfigManager::getBluetoothMac();
    if (storedMac.length() && parseMac(storedMac, addr)) {
        a2dp_source.set_auto_reconnect(addr);
    }

    String storedName = ConfigManager::getBluetoothName();
    a2dp_source.start(storedName.c_str());
}

void BtAudio::loop() {
    // A2DP runs in its own task, nothing to do here currently.
}

void BtAudio::play(const char* path) {
    stop();

    currentFile = LittleFS.open(path, "r");
    if (!currentFile) {
        Serial.printf("[BtAudio] ERROR: File %s could not be opened\n", path);
        return;
    }

    decoder.begin();
    playing = true;
    Serial.printf("[BtAudio] Starting playback: %s\n", path);
}

void BtAudio::stop() {
    if (playing) {
        playing = false;
        currentFile.close();
        Serial.println("[BtAudio] Playback stopped");
    }
}

bool BtAudio::isPlaying() {
    return playing;
}

bool BtAudio::isConnected() {
    return connected;
}
