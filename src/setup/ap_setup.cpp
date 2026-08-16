#include "ap_setup.h"

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "../config/config_manager.h"

namespace {
    // Open, see docs/architektur.md. Add a password via
    // WiFi.softAP(AP_SSID, "password") if needed.
    constexpr const char* AP_SSID = "Paintball-Setup";

    constexpr const char* MP3_DIR = "/mp3";
    // Must match the limit in setup.html's JS.
    constexpr size_t MAX_TOTAL_MP3_BYTES = 1500000UL;

    AsyncWebServer server(80);

    File uploadFile;
    size_t bytesWrittenThisUpload = 0;
    size_t otherFilesSizeAtUploadStart = 0;
    bool uploadRejected = false;
    String uploadRejectReason;

    bool rebootRequested = false;
    unsigned long rebootAtMs = 0;

    String basename(const String& path) {
        int slash = path.lastIndexOf('/');
        return slash < 0 ? path : path.substring(slash + 1);
    }

    size_t mp3DirectorySize(const String& excludeFilename = String()) {
        size_t total = 0;
        File dir = LittleFS.open(MP3_DIR);
        if (!dir || !dir.isDirectory()) return 0;

        File f = dir.openNextFile();
        while (f) {
            if (!f.isDirectory() && basename(f.name()) != excludeFilename) {
                total += f.size();
            }
            f = dir.openNextFile();
        }
        return total;
    }

    void handleMp3Upload(AsyncWebServerRequest* /*request*/, const String& filename,
                          size_t index, uint8_t* data, size_t len, bool final) {
        if (index == 0) {
            uploadRejected = false;
            uploadRejectReason = "";
            bytesWrittenThisUpload = 0;
            otherFilesSizeAtUploadStart = mp3DirectorySize(filename);

            if (!LittleFS.exists(MP3_DIR)) {
                LittleFS.mkdir(MP3_DIR);
            }
            uploadFile = LittleFS.open(String(MP3_DIR) + "/" + filename, "w");
            if (!uploadFile) {
                uploadRejected = true;
                uploadRejectReason = "FILE_CREATE_FAILED";
                return;
            }
        }

        if (uploadRejected) return;

        if (otherFilesSizeAtUploadStart + bytesWrittenThisUpload + len > MAX_TOTAL_MP3_BYTES) {
            uploadRejected = true;
            uploadRejectReason = "SIZE_EXCEEDED";
            if (uploadFile) {
                uploadFile.close();
                LittleFS.remove(String(MP3_DIR) + "/" + filename);
            }
            return;
        }

        uploadFile.write(data, len);
        bytesWrittenThisUpload += len;

        if (final) {
            uploadFile.close();
            Serial.printf("[ApSetup] Upload complete: %s (%u bytes)\n",
                           filename.c_str(), (unsigned)bytesWrittenThisUpload);
        }
    }

    void handleUploadComplete(AsyncWebServerRequest* request) {
        if (uploadRejected) {
            request->send(413, "text/plain", uploadRejectReason);
        } else {
            request->send(200, "text/plain", "OK");
        }
    }

    void handleFilesList(AsyncWebServerRequest* request) {
        JsonDocument doc;
        JsonArray files = doc["files"].to<JsonArray>();
        size_t total = 0;

        File dir = LittleFS.open(MP3_DIR);
        if (dir && dir.isDirectory()) {
            File f = dir.openNextFile();
            while (f) {
                if (!f.isDirectory()) {
                    JsonObject entry = files.add<JsonObject>();
                    entry["name"] = basename(f.name());
                    entry["size"] = f.size();
                    total += f.size();
                }
                f = dir.openNextFile();
            }
        }
        doc["total"] = total;
        doc["max"] = MAX_TOTAL_MP3_BYTES;

        String out;
        serializeJson(doc, out);
        request->send(200, "application/json", out);
    }

    void handleMappingGet(AsyncWebServerRequest* request) {
        request->send(200, "application/json", ConfigManager::getMappingJson());
    }

    void handleBtNameGet(AsyncWebServerRequest* request) {
        request->send(200, "text/plain", ConfigManager::getBluetoothName());
    }

    void handleDelete(AsyncWebServerRequest* request) {
        String name = request->hasParam("name", true)
            ? request->getParam("name", true)->value()
            : "";

        // Reject path separators - name is used to build a filesystem path below.
        if (name.length() == 0 || name.indexOf('/') >= 0) {
            request->send(400, "text/plain", "INVALID_NAME");
            return;
        }

        String path = String(MP3_DIR) + "/" + name;
        if (!LittleFS.exists(path)) {
            request->send(404, "text/plain", "NOT_FOUND");
            return;
        }

        LittleFS.remove(path);
        Serial.printf("[ApSetup] Deleted: %s\n", name.c_str());
        request->send(200, "text/plain", "DELETED");
    }

    void handleSave(AsyncWebServerRequest* request) {
        String btName = request->hasParam("bt_name", true)
            ? request->getParam("bt_name", true)->value()
            : "";
        String mappingJson = request->hasParam("mapping_json", true)
            ? request->getParam("mapping_json", true)->value()
            : "";

        if (btName.length() == 0) {
            request->send(400, "text/plain", "BT_NAME_REQUIRED");
            return;
        }

        JsonDocument doc;
        if (mappingJson.length() == 0 || deserializeJson(doc, mappingJson)) {
            request->send(400, "text/plain", "MAPPING_INVALID");
            return;
        }

        ConfigManager::setBluetoothName(btName);
        ConfigManager::setMappingJson(mappingJson);
        ConfigManager::setConfigured(true);

        request->send(200, "text/plain", "SAVED");

        // Delayed so the response still reaches the client.
        rebootRequested = true;
        rebootAtMs = millis() + 1000;
    }
}

void ApSetup::begin() {
    // partitionLabel, see docs/troubleshooting.md (spiffs-vs-littlefs pitfall).
    if (!LittleFS.begin(true, "/littlefs", 10, "littlefs")) {
        Serial.println("[ApSetup] ERROR: LittleFS could not be started");
    }
    if (!LittleFS.exists(MP3_DIR)) {
        LittleFS.mkdir(MP3_DIR);
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    Serial.printf("[ApSetup] AP started: SSID=%s IP=%s\n", AP_SSID, WiFi.softAPIP().toString().c_str());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/setup.html", "text/html");
    });
    server.on("/files", HTTP_GET, handleFilesList);
    server.on("/mapping", HTTP_GET, handleMappingGet);
    server.on("/bt_name", HTTP_GET, handleBtNameGet);
    server.on("/upload", HTTP_POST, handleUploadComplete, handleMp3Upload);
    server.on("/delete", HTTP_POST, handleDelete);
    server.on("/save", HTTP_POST, handleSave);

    server.begin();
}

void ApSetup::loop() {
    if (rebootRequested && millis() >= rebootAtMs) {
        ESP.restart();
    }
}
