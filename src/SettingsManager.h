// SettingsManager.h
#pragma once

#include <SDL2/SDL.h>
#include <string>

struct settings {
    std::string sndTheme, gfxTheme, lvlTheme;
    bool cfgRes[2];
    int displayToUse;
    int res_x;
    int res_y;
    mutable int fps;
    bool showClock;
    bool fullscreen;
    bool showBg;
    bool showBgOverlay;
    bool sound;
    bool stereo;
    bool eyeCandy;
    bool particleCollide;
    SDL_Keycode keyLeft, keyRight, keyShoot, keyNextPo, keyPrevPo, keyBuyPo;
    float controlAccel;
    float controlStartSpeed;
    float controlMaxSpeed;
    bool joyEnabled, joyIsDigital, joyIsPaddle;
    int JoyCalMin, JoyCalMax, JoyCalHighJitter, JoyCalLowJitter;
    int startingDifficulty;
};

class SettingsManager {
    settings defaultSettings{}; // Compile-time Defaults aus config.h
    settings loadedSettings{}; // Aus Datei geladene Settings
    settings currentSettings{}; // Aktuelle Runtime-Settings
    std::string settingsFilePath;

    void setDefaults();

    bool loadSettings();

    bool validateSettings();

    [[nodiscard]] bool writeSettings() const;

    [[nodiscard]] bool hasChanged() const;

public:
    explicit SettingsManager(const std::string &settingsFile);

    // Verhindere Kopieren
    SettingsManager(const SettingsManager &) = delete;

    SettingsManager &operator=(const SettingsManager &) = delete;

    bool init();

    void resetToDefaults() { currentSettings = defaultSettings; }
    void resetToLoaded() { currentSettings = loadedSettings; }

    [[nodiscard]] int getDifficulty() const {
        return currentSettings.startingDifficulty;
    }

    [[nodiscard]] const settings &getSettings() const {
        return currentSettings;
    }

    [[nodiscard]] bool isFullscreen() const {
        return currentSettings.fullscreen;
    }

    void setFullscreen(bool value);

    ~SettingsManager();
};
