#pragma once

// A2DP source + MP3 playback from LittleFS (arduino-audio-tools), see docs/architektur.md.
namespace BtAudio {
    void begin();
    void loop();

    // path e.g. "/mp3/audio1.mp3". Stops any ongoing playback first.
    void play(const char* path);

    void stop();
    bool isPlaying();
    bool isConnected();
}
