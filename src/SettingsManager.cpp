// settings_manager.cpp

#include "SettingsManager.h"
#include <SDL2/SDL.h>
#include "config.h"
#include "game_state.h"

void SettingsManager::setDefaults() {
    defaultSettings.gfxTheme = DEFAULT_GFX_THEME;
    defaultSettings.sndTheme = DEFAULT_SND_THEME;
    defaultSettings.lvlTheme = DEFAULT_LVL_THEME;
    defaultSettings.eyeCandy = DEFAULT_EYE_CANDY;
    defaultSettings.showBg = DEFAULT_SHOW_BG;
    defaultSettings.particleCollide = DEFAULT_PARTICLE_COLLIDE;

    defaultSettings.sound = DEFAULT_SOUND;
    defaultSettings.stereo = DEFAULT_STEREO;

    defaultSettings.keyLeft = DEFAULT_KEY_LEFT;
    defaultSettings.keyRight = DEFAULT_KEY_RIGHT;
    defaultSettings.keyShoot = DEFAULT_KEY_SHOOT;
    defaultSettings.keyNextPo = static_cast<SDL_Keycode>(DEFAULT_KEY_NEXT_PO);
    defaultSettings.keyBuyPo = static_cast<SDL_Keycode>(DEFAULT_KEY_BUY_PO);
    defaultSettings.keyPrevPo = static_cast<SDL_Keycode>(DEFAULT_KEY_PREV_PO);

    defaultSettings.controlAccel = DEFAULT_CONTROL_ACCEL;
    defaultSettings.controlStartSpeed = DEFAULT_CONTROL_START_SPEED;
    defaultSettings.controlMaxSpeed = DEFAULT_CONTROL_MAX_SPEED;

    defaultSettings.joyEnabled = DEFAULT_JOY_ENABLED;
    defaultSettings.joyIsDigital = DEFAULT_JOY_IS_DIGITAL;
    defaultSettings.joyIsPaddle = DEFAULT_JOY_IS_PADDLE;
    defaultSettings.showClock = DEFAULT_SHOW_CLOCK;

    defaultSettings.JoyCalMin = DEFAULT_JOY_CAL_MIN;
    defaultSettings.JoyCalMax = DEFAULT_JOY_CAL_MAX;
    defaultSettings.JoyCalLowJitter = DEFAULT_JOY_CAL_LOW;
    defaultSettings.JoyCalHighJitter = DEFAULT_JOY_CAL_HIGH;

    defaultSettings.fullscreen = DEFAULT_FULLSCREEN;
    defaultSettings.fps = DEFAULT_FPS;
    defaultSettings.res_x = DEFAULT_RES_X;
    defaultSettings.res_y = DEFAULT_RES_Y;
    defaultSettings.startingDifficulty = DEFAULT_DIFFICULTY;

    currentSettings = defaultSettings;
}

bool SettingsManager::init() {
    if (!loadSettings()) {
        resetToDefaults();
        return writeSettings();
    }
    validateSettings();
    resetToLoaded();
    return true;
}

bool SettingsManager::loadSettings() {
    std::ifstream file(configFileManager.getSettingsFile());
    if (!file.is_open()) {
        return false;
    }

    std::string line, key, value;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        key = line.substr(0, pos);
        value = line.substr(pos + 1);

        // Einstellungen einlesen
        if (key == "gfxtheme") loadedSettings.gfxTheme = value;
        else if (key == "sndtheme") loadedSettings.sndTheme = value;
        else if (key == "lvltheme") loadedSettings.lvlTheme = value;
        else if (key == "eyecandy") loadedSettings.eyeCandy = std::stoi(value);
        else if (key == "display") loadedSettings.displayToUse = std::stoi(value);
        else if (key == "fps") loadedSettings.fps = std::stoi(value);
        else if (key == "res_x") loadedSettings.res_x = std::stoi(value);
        else if (key == "res_y") loadedSettings.res_y = std::stoi(value);
        else if (key == "showclock") loadedSettings.showClock = std::stoi(value);
        else if (key == "fullscreen") loadedSettings.fullscreen = std::stoi(value);
        else if (key == "showbg") loadedSettings.showBg = std::stoi(value);
        else if (key == "sound") loadedSettings.sound = std::stoi(value);
        else if (key == "stereo") loadedSettings.stereo = std::stoi(value);
        else if (key == "particlecollide") loadedSettings.particleCollide = std::stoi(value);

        // Controls
        else if (key == "keyleft") loadedSettings.keyLeft = static_cast<SDL_Keycode>(std::stoi(value));
        else if (key == "keyright") loadedSettings.keyRight = static_cast<SDL_Keycode>(std::stoi(value));
        else if (key == "keyshoot") loadedSettings.keyShoot = static_cast<SDL_Keycode>(std::stoi(value));
        else if (key == "keynextpo") loadedSettings.keyNextPo = static_cast<SDL_Keycode>(std::stoi(value));
        else if (key == "keyprevpo") loadedSettings.keyPrevPo = static_cast<SDL_Keycode>(std::stoi(value));
        else if (key == "keybuypo") loadedSettings.keyBuyPo = static_cast<SDL_Keycode>(std::stoi(value));

        // Control speeds
        else if (key == "controlaccel") loadedSettings.controlAccel = std::stof(value);
        else if (key == "controlstartspeed") loadedSettings.controlStartSpeed = std::stof(value);
        else if (key == "controlmaxspeed") loadedSettings.controlMaxSpeed = std::stof(value);

        // Joystick
        else if (key == "joyenabled") loadedSettings.joyEnabled = std::stoi(value);
        else if (key == "joyisdigital") loadedSettings.joyIsDigital = std::stoi(value);
        else if (key == "joyispaddle") loadedSettings.joyIsPaddle = std::stoi(value);
        else if (key == "joycalmin") loadedSettings.JoyCalMin = std::stoi(value);
        else if (key == "joycalmax") loadedSettings.JoyCalMax = std::stoi(value);
        else if (key == "joycallow") loadedSettings.JoyCalLowJitter = std::stoi(value);
        else if (key == "joycalhigh") loadedSettings.JoyCalHighJitter = std::stoi(value);
        else if (key == "startingdifficulty") loadedSettings.startingDifficulty = std::stoi(value);
    }
    file.close();
    return true;
}

bool SettingsManager::validateSettings() {
    // Auflösung validieren
    // Wenn keine Auflösung gesetzt ist, Vollbildmodus aktivieren
    if (loadedSettings.res_x == 0 || loadedSettings.res_y == 0) {
        loadedSettings.fullscreen = true;
        loadedSettings.res_x = 0;
        loadedSettings.res_y = 0;
    }

    // Auflösung validieren
    if (loadedSettings.fullscreen) {
        // Im Vollbildmodus keine Auflösungsprüfung nötig,
        // da SDL die native Auflösung verwendet
        loadedSettings.res_x = 0;
        loadedSettings.res_y = 0;
    } else {
        // Im Fenstermodus muss die Auflösung in vernünftigen Grenzen liegen
        if (loadedSettings.res_x < 320 || loadedSettings.res_x > 7680 ||
            loadedSettings.res_y < 240 || loadedSettings.res_y > 4320) {
            SDL_Log("Ungültige Auflösung: %dx%d",
                    loadedSettings.res_x, loadedSettings.res_y);
            return false;
        }
    }

    // Schwierigkeitsgrad validieren
    if (loadedSettings.startingDifficulty < EASY ||
        loadedSettings.startingDifficulty > HARD) {
        SDL_Log("Ungültiger Schwierigkeitsgrad %d, setze auf %d",
                loadedSettings.startingDifficulty, DEFAULT_DIFFICULTY);
        loadedSettings.startingDifficulty = DEFAULT_DIFFICULTY;
    }

    // oO ... weitere Validierungen

    return true;
}

bool SettingsManager::writeSettings() const {
    std::ofstream file(configFileManager.getSettingsFile());
    if (!file.is_open()) {
        return false;
    }

    file << "gfxtheme=" << currentSettings.gfxTheme << "\n"
            << "sndtheme=" << currentSettings.sndTheme << "\n"
            << "lvltheme=" << currentSettings.lvlTheme << "\n"
            << "eyecandy=" << currentSettings.eyeCandy << "\n"
            << "display=" << currentSettings.displayToUse << "\n"
            << "res_x=" << currentSettings.res_x << "\n"
            << "res_y=" << currentSettings.res_y << "\n"
            << "fps=" << currentSettings.fps << "\n"
            << "showclock=" << currentSettings.showClock << "\n"
            << "fullscreen=" << currentSettings.fullscreen << "\n"
            << "showbg=" << currentSettings.showBg << "\n"
            << "sound=" << currentSettings.sound << "\n"
            << "stereo=" << currentSettings.stereo << "\n"
            << "particlecollide=" << currentSettings.particleCollide << "\n"

            // Controls
            << "keyleft=" << currentSettings.keyLeft << "\n"
            << "keyright=" << currentSettings.keyRight << "\n"
            << "keyshoot=" << currentSettings.keyShoot << "\n"
            << "keynextpo=" << currentSettings.keyNextPo << "\n"
            << "keyprevpo=" << currentSettings.keyPrevPo << "\n"
            << "keybuypo=" << currentSettings.keyBuyPo << "\n"

            // Control speeds
            << "controlaccel=" << currentSettings.controlAccel << "\n"
            << "controlstartspeed=" << currentSettings.controlStartSpeed << "\n"
            << "controlmaxspeed=" << currentSettings.controlMaxSpeed << "\n"

            // Joystick
            << "joyenabled=" << currentSettings.joyEnabled << "\n"
            << "joyisdigital=" << currentSettings.joyIsDigital << "\n"
            << "joyispaddle=" << currentSettings.joyIsPaddle << "\n"
            << "joycalmin=" << currentSettings.JoyCalMin << "\n"
            << "joycalmax=" << currentSettings.JoyCalMax << "\n"
            << "joycallow=" << currentSettings.JoyCalLowJitter << "\n"
            << "joycalhigh=" << currentSettings.JoyCalHighJitter << "\n"
            << "startingdifficulty=" << currentSettings.startingDifficulty << "\n";
    file.close();
    return true;
}

void SettingsManager::setFullscreen(const bool value) {
    if (value != currentSettings.fullscreen) {
        currentSettings.fullscreen = value;
        if (value) {
            currentSettings.res_x = 0;
            currentSettings.res_y = 0;
        }
        else { // TODO: try loaded settings
            currentSettings.res_x = DEFAULT_RES_X;
            currentSettings.res_y = DEFAULT_RES_Y;
        }
    }
}

SettingsManager::~SettingsManager() {
    if (hasChanged()) {
        if (!writeSettings()) {;
            SDL_Log("Failed to write settings to file");
        }
    }
}

bool SettingsManager::hasChanged() const {
    return currentSettings.sndTheme != loadedSettings.sndTheme
        || currentSettings.gfxTheme != loadedSettings.gfxTheme
        || currentSettings.lvlTheme != loadedSettings.lvlTheme
        || currentSettings.displayToUse != loadedSettings.displayToUse
        || currentSettings.res_x != loadedSettings.res_x
        || currentSettings.res_y != loadedSettings.res_y
        || currentSettings.fps != loadedSettings.fps
        || currentSettings.showClock != loadedSettings.showClock
        || currentSettings.fullscreen != loadedSettings.fullscreen
        || currentSettings.showBg != loadedSettings.showBg
        || currentSettings.sound != loadedSettings.sound
        || currentSettings.stereo != loadedSettings.stereo
        || currentSettings.eyeCandy != loadedSettings.eyeCandy
        || currentSettings.particleCollide != loadedSettings.particleCollide
        || currentSettings.keyLeft != loadedSettings.keyLeft
        || currentSettings.keyRight != loadedSettings.keyRight
        || currentSettings.keyShoot != loadedSettings.keyShoot
        || currentSettings.keyNextPo != loadedSettings.keyNextPo
        || currentSettings.keyPrevPo != loadedSettings.keyPrevPo
        || currentSettings.keyBuyPo != loadedSettings.keyBuyPo
        || currentSettings.controlAccel != loadedSettings.controlAccel
        || currentSettings.controlStartSpeed != loadedSettings.controlStartSpeed
        || currentSettings.controlMaxSpeed != loadedSettings.controlMaxSpeed
        || currentSettings.joyEnabled != loadedSettings.joyEnabled
        || currentSettings.joyIsDigital != loadedSettings.joyIsDigital
        || currentSettings.joyIsPaddle != loadedSettings.joyIsPaddle
        || currentSettings.JoyCalMin != loadedSettings.JoyCalMin
        || currentSettings.JoyCalMax != loadedSettings.JoyCalMax
        || currentSettings.JoyCalHighJitter != loadedSettings.JoyCalHighJitter
        || currentSettings.JoyCalLowJitter != loadedSettings.JoyCalLowJitter
        || currentSettings.startingDifficulty != loadedSettings.startingDifficulty;
}
