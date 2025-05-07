// settings_manager.cpp
#include "settings_manager.h"
#include <SDL2/SDL.h>
#include "config.h"

SettingsManager::SettingsManager(ConfigFile &cf) : currentSettings(), configFile(cf) {
    hasChanges = false;
}

void SettingsManager::setDefaults() {
    currentSettings.gfxTheme = DEFAULT_GFX_THEME;
    currentSettings.sndTheme = DEFAULT_SND_THEME;
    currentSettings.lvlTheme = DEFAULT_LVL_THEME;
    currentSettings.eyeCandy = DEFAULT_EYE_CANDY;
    currentSettings.showBg = DEFAULT_SHOW_BG;
    currentSettings.particleCollide = DEFAULT_PARTICLE_COLLIDE;

    currentSettings.sound = DEFAULT_SOUND;
    currentSettings.stereo = DEFAULT_STEREO;

    currentSettings.keyLeft = DEFAULT_KEY_LEFT;
    currentSettings.keyRight = DEFAULT_KEY_RIGHT;
    currentSettings.keyShoot = DEFAULT_KEY_SHOOT;
    currentSettings.keyNextPo = static_cast<SDL_Keycode>(DEFAULT_KEY_NEXT_PO);
    currentSettings.keyBuyPo = static_cast<SDL_Keycode>(DEFAULT_KEY_BUY_PO);
    currentSettings.keyPrevPo = static_cast<SDL_Keycode>(DEFAULT_KEY_PREV_PO);

    currentSettings.controlAccel = DEFAULT_CONTROL_ACCEL;
    currentSettings.controlStartSpeed = DEFAULT_CONTROL_START_SPEED;
    currentSettings.controlMaxSpeed = DEFAULT_CONTROL_MAX_SPEED;

    currentSettings.joyEnabled = DEFAULT_JOY_ENABLED;
    currentSettings.joyIsDigital = DEFAULT_JOY_IS_DIGITAL;
    currentSettings.joyIsPaddle = DEFAULT_JOY_IS_PADDLE;
    currentSettings.showClock = DEFAULT_SHOW_CLOCK;

    currentSettings.JoyCalMin = DEFAULT_JOY_CAL_MIN;
    currentSettings.JoyCalMax = DEFAULT_JOY_CAL_MAX;
    currentSettings.JoyCalLowJitter = DEFAULT_JOY_CAL_LOW;
    currentSettings.JoyCalHighJitter = DEFAULT_JOY_CAL_HIGH;

    currentSettings.fullscreen = DEFAULT_FULLSCREEN;
    currentSettings.fps = DEFAULT_FPS;
    currentSettings.res_x = DEFAULT_RES_X;
    currentSettings.res_y = DEFAULT_RES_Y;
    currentSettings.startingDifficulty = DEFAULT_DIFFICULTY;
}

bool SettingsManager::init() {
    setDefaults(); // Immer zuerst Defaults setzen
    if (!readFromFile()) {
        // Wenn Datei nicht existiert oder fehlerhaft,
        // Default-Werte behalten und neu speichern
        return writeToFile();
    }
    return validateSettings();
}

bool SettingsManager::readFromFile() {
    std::ifstream file(configFile.getSettingsFile());
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
        if (key == "gfxtheme") currentSettings.gfxTheme = value;
        else if (key == "sndtheme") currentSettings.sndTheme = value;
        else if (key == "lvltheme") currentSettings.lvlTheme = value;
        else if (key == "eyecandy") currentSettings.eyeCandy = std::stoi(value);
        else if (key == "display") currentSettings.displayToUse = std::stoi(value);
        else if (key == "fps") currentSettings.fps = std::stoi(value);
        else if (key == "res_x") currentSettings.res_x = std::stoi(value);
        else if (key == "res_y") currentSettings.res_y = std::stoi(value);
        else if (key == "showclock") currentSettings.showClock = std::stoi(value);
        else if (key == "fullscreen") currentSettings.fullscreen = std::stoi(value);
        else if (key == "showbg") currentSettings.showBg = std::stoi(value);
        else if (key == "sound") currentSettings.sound = std::stoi(value);
        else if (key == "stereo") currentSettings.stereo = std::stoi(value);
        else if (key == "particlecollide") currentSettings.particleCollide = std::stoi(value);
        else if (key == "startingdifficulty")
            currentSettings.startingDifficulty = std::stoi(value);
    }

    file.close();
    return true;
}

bool SettingsManager::validateSettings() {
    // Auflösung validieren
    // Wenn keine Auflösung gesetzt ist, Vollbildmodus aktivieren
    if (currentSettings.res_x == 0 || currentSettings.res_y == 0) {
        currentSettings.fullscreen = true;
        currentSettings.res_x = 0;
        currentSettings.res_y = 0;
    }

    // Auflösung validieren
    if (currentSettings.fullscreen) {
        // Im Vollbildmodus keine Auflösungsprüfung nötig,
        // da SDL die native Auflösung verwendet
        currentSettings.res_x = 0;
        currentSettings.res_y = 0;
    } else {
        // Im Fenstermodus muss die Auflösung in vernünftigen Grenzen liegen
        if (currentSettings.res_x < 320 || currentSettings.res_x > 7680 ||
            currentSettings.res_y < 240 || currentSettings.res_y > 4320) {
            SDL_Log("Ungültige Auflösung: %dx%d",
                    currentSettings.res_x, currentSettings.res_y);
            return false;
        }
    }

    // Schwierigkeitsgrad validieren
    if (currentSettings.startingDifficulty < EASY ||
        currentSettings.startingDifficulty > HARD) {
        SDL_Log("Ungültiger Schwierigkeitsgrad %d, setze auf %d",
                currentSettings.startingDifficulty, DEFAULT_DIFFICULTY);
        currentSettings.startingDifficulty = DEFAULT_DIFFICULTY;
    }

    // ... weitere Validierungen

    return true;
}

bool SettingsManager::writeToFile() const {
    std::ofstream file(configFile.getSettingsFile());
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

bool SettingsManager::save() {
    if (!hasChanges) return true;
    if (writeToFile()){
        hasChanges = false;
        return true;
    }
    return false;
}

const settings &SettingsManager::getSettings() const {
    return currentSettings;
}

settings &SettingsManager::getSettings() {
    return currentSettings;
}
