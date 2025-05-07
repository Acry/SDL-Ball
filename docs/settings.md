# Settings

## more robust and flexible implementation:

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
