# Settings

## old settings explained

README: 1.4.6 Config options explained

- clearscreen [0/1] - clears the scene before drawing (looks pretty funny if you dont)
- eyecandy [0/1] - turns on nice particle effects and trails, saves a few cpu cycles to turn it off
- resx - horizontal resolution
- resy - vertical resolution
- showbg [0/1] - show the background image
- fullscreen [0/1] - use full-screen mode
- particlecollide [0/1] - use collision detection for particles, looks nice but might a cpu hog
- sound [0/1] - use sound effects (sound effects will allways be loaded) (requires compiled with sound support)
- stereo [0/1] - use stereo, some people don't like stereo, might save a few cpu cycles to turn it off
- controlmaxspeed - the max speed the paddle can travel with when using non-mouse controllers.
- 10 is VERY fast, this acts as sensitivity for WiiMote and analog joystick.
- controlaccel - the acceleration that the paddle gains when using digital joystick or keyboard for controlling.
- controlstartspeed - the starting speed (the "lowest" speed) of the paddle when using digital joystick or keyboard.
- rightkey - SDLK number of the key that makes the paddle travel right
- leftkey - same as above, but left.
- nextkey - Select next powerup.
- buykey - Buy selected powerup.
- prevkey - Select previous powerup.
- shootkey - the key that makes the guns shoot and releases the ball.
- Check this site for sdl key numbers: http://kobesearch.cpan.org/htdocs/SDL_Perl/SDL/Constants.pm.html#SDLK_0-
- joyisdigital [0/1] - if this is 1, the joystick acts as a "dump"/digital joystick (behaving excatly like the keyboard)
- if this is 0, the joystick acts as a analog/precise joystick.
- joycal* - callibration values from the joystick callibration process.
- joyenabled [1/0] - if 1 then joystick is enabled (set to 0 if you experience random movement/controlling error)
- sndtheme - Case sensetive name of the sound-theme dir to use.
- gfxtheme - Case sensetive name of the gfx-theme dir to use.
- lvltheme - Case sensetive name of the level-theme dir to use.

## Key-Value System

Ja, eine bessere Lösung wäre ein flexibleres Key-Value System zu implementieren. Hier ein Vorschlag:

1. Erst eine Map für die Settings definieren:

```c++
// settings_manager.h
#include <map>
#include <variant>

using SettingValue = std::variant<std::string, int, bool, float, SDL_Keycode>;
using SettingsMap = std::map<std::string, SettingValue>;

class SettingsManager {
private:
    SettingsMap settingsMap;
    ConfigFile &configFile;
    
    void setDefaults();
    bool readFromFile();
    bool writeToFile() const;
    bool validateSettings();
    
    // Hilfsfunktion zum Type-Safe Zugriff
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
public:
    // ... rest wie gehabt ...
};
```

2. Defaults und Zugriff vereinfachen:

```c++
// settings_manager.cpp
void SettingsManager::setDefaults() {
    settingsMap = {
        {"gfxtheme", std::string(DEFAULT_GFX_THEME)},
        {"sndtheme", std::string(DEFAULT_SND_THEME)},
        {"lvltheme", std::string(DEFAULT_LVL_THEME)},
        {"eyecandy", DEFAULT_EYE_CANDY},
        {"fps", DEFAULT_FPS},
        {"fullscreen", DEFAULT_FULLSCREEN},
        // ... weitere defaults ...
    };
}

bool SettingsManager::readFromFile() {
    std::ifstream file(configFile.getSettingsFile());
    if (!file.is_open()) return false;

    std::string line, key, value;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        key = line.substr(0, pos);
        value = line.substr(pos + 1);

        // Typ aus Map ermitteln und konvertieren
        if (settingsMap.contains(key)) {
            try {
                if (std::holds_alternative<std::string>(settingsMap[key])) {
                    settingsMap[key] = value;
                } else if (std::holds_alternative<int>(settingsMap[key])) {
                    settingsMap[key] = std::stoi(value);
                } else if (std::holds_alternative<bool>(settingsMap[key])) {
                    settingsMap[key] = std::stoi(value) != 0;
                } else if (std::holds_alternative<float>(settingsMap[key])) {
                    settingsMap[key] = std::stof(value);
                }
            } catch (...) {
                // Bei Konvertierungsfehler Default behalten
            }
        }
    }
    return true;
}

bool SettingsManager::writeToFile() const {
    std::ofstream file(configFile.getSettingsFile());
    if (!file.is_open()) return false;

    for (const auto& [key, value] : settingsMap) {
        file << key << "=";
        std::visit([&file](const auto& v) { file << v; }, value);
        file << "\n";
    }
    return true;
}
```

3. Getter/Setter für die Settings:

```c++
template<typename T>
T SettingsManager::getValue(const std::string& key, const T& defaultValue) const {
    if (!settingsMap.contains(key)) return defaultValue;
    if (auto value = std::get_if<T>(&settingsMap.at(key))) {
        return *value;
    }
    return defaultValue;
}

// Beispiel für die settings struct
const settings& SettingsManager::getSettings() const {
    settings result;
    result.gfxTheme = getValue<std::string>("gfxtheme", DEFAULT_GFX_THEME);
    result.fps = getValue<int>("fps", DEFAULT_FPS);
    result.fullscreen = getValue<bool>("fullscreen", DEFAULT_FULLSCREEN);
    // ... etc ...
    return result;
}
```

Vorteile dieser Lösung:
- Zentrale Definition aller Settings
- Automatische Typ-Sicherheit
- Einfaches Hinzufügen/Entfernen von Settings
- Selbstdokumentierender Code durch die Map-Definition
- Weniger Fehleranfällig bei Änderungen
- Bessere Wartbarkeit

Der einzige Nachteil ist die etwas komplexere Implementation, aber das wird durch die bessere Wartbarkeit mehr als ausgeglichen.
