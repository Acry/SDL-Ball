// settings_manager.h
#pragma once

#include "config_file.h"
#include <string>
#include <fstream>
#include <SDL2/SDL_keycode.h>

struct settings {
    std::string sndTheme, gfxTheme, lvlTheme;
    bool cfgRes[2];
    int displayToUse;
    int res_x;
    int res_y;
    int fps;
    bool showClock;
    bool fullscreen;
    bool showBg;
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

extern settings setting;
class SettingsManager {
    settings currentSettings;
    ConfigFile &configFile;

    bool hasChanges;

    void setDefaults();

    bool readFromFile();

    bool writeToFile() const;

    bool validateSettings();

public:
    explicit SettingsManager(ConfigFile &cf);

    ~SettingsManager() = default;

    bool init();

    bool save();

    bool hasChanged() const { return hasChanges; }
    void settingsChanged() { hasChanges = true; }

    const settings &getSettings() const;

    settings &getSettings();
};
