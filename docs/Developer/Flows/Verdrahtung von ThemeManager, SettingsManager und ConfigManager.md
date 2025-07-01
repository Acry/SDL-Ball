# Verdrahtung von ThemeManager, SettingsManager und ConfigManager

Basierend auf den aktuellen Implementierungen schlage ich folgendes Konzept für die Zuständigkeiten und Verdrahtung der Manager vor:

## Zuständigkeiten

1. **ConfigFileManager**:
    - Verwaltet Pfade und Verzeichnisse
    - Stellt Basispfade für andere Manager bereit
    - Kennt keine anderen Manager

2. **SettingsManager**:
    - Verwaltet Benutzereinstellungen, einschließlich Theme-Präferenzen
    - Lädt/speichert Einstellungen in Dateien
    - Nutzt ConfigFileManager für Dateipfade

3. **ThemeManager**:
    - Erkennt verfügbare Themes
    - Verwaltet aktuelles Theme basierend auf Einstellungen
    - Stellt Theme-Ressourcenpfade bereit
    - Nutzt ConfigFileManager für Theme-Verzeichnisse

## Verdrahtung und Initialization

```cpp
// Initialization-Sequenz
ConfigFileManager configManager;
SettingsManager settingsManager(configManager);
ThemeManager themeManager(configManager);

// Aktuelles Theme aus den Settings setzen
themeManager.setCurrentTheme(settingsManager.getSettings().gfxTheme);
```

## Anpassungen am ThemeManager

Wir benötigen folgende Änderungen am ThemeManager:

```cpp
// ThemeManager.h
class ThemeManager {
public:
    explicit ThemeManager(const ConfigFileManager& configFileManager);
    
    // Bestehende Methoden
    std::vector<themeInfo> listThemes();
    bool themeExists(const std::string& name) const;
    std::string getThemeFilePath(const std::string& path, const std::string& theme = "") const;
    static std::string getDefaultTheme();
    
    // Neue Methoden
    bool setCurrentTheme(const std::string& themeName);
    std::string getCurrentTheme() const;

private:
    std::vector<themeInfo> themes;
    std::string currentTheme; // Aktuelles Theme
    const ConfigFileManager& configFileManager;
    void scanThemes();
};
```

```cpp
// ThemeManager.cpp (neue Methoden)
bool ThemeManager::setCurrentTheme(const std::string &themeName) {
    if (themeExists(themeName)) {
        currentTheme = themeName;
        return true;
    }
    // Fallback auf Standard-Theme
    currentTheme = getDefaultTheme();
    return false;
}

std::string ThemeManager::getCurrentTheme() const {
    return currentTheme;
}

// Konstruktor-Anpassung
ThemeManager::ThemeManager(const ConfigFileManager &configFileManager) 
    : configFileManager(configFileManager), currentTheme(getDefaultTheme()) {
    scanThemes();
}

// getThemeFilePath-Anpassung
std::string ThemeManager::getThemeFilePath(const std::string &path, const std::string &theme) const {
    // Theme-Parameter nimmt Vorrang vor currentTheme
    std::string themeToUse = theme.empty() ? currentTheme : theme;
    // Rest der Methode bleibt gleich...
}
```

## Erweiterung für getrennte Theme-Arten

Falls wir bei der ursprünglichen Idee bleiben möchten, dass Grafik-, Sound- und Level-Themes getrennt wählbar sind:

```cpp
// ThemeManager.h - Erweitere um Theme-Typen
enum class ThemeType {
    GRAPHICS,
    SOUND,
    LEVEL
};

class ThemeManager {
public:
    // ...bestehende Methoden...
    
    bool setCurrentTheme(const std::string& themeName, ThemeType type);
    std::string getCurrentTheme(ThemeType type) const;
    std::string getThemeFilePath(const std::string& path, ThemeType type, const std::string& theme = "") const;

private:
    // ...
    std::string currentGfxTheme;
    std::string currentSndTheme;
    std::string currentLvlTheme;
};
```

## Anwendung im Code

```cpp
// Beispielanwendung
void loadGameResources() {
    // Paddle-Textur aus dem Grafik-Theme laden
    std::string paddlePath = themeManager.getThemeFilePath("gfx/paddle/base.png");
    
    // Spezifisches Sound-Theme verwenden
    std::string soundPath = themeManager.getThemeFilePath("snd/hit.wav", ThemeType::SOUND);
    
    // Level aus aktuellem Level-Theme laden
    std::string levelPath = themeManager.getThemeFilePath("levels.txt", ThemeType::LEVEL);
}

// Einstellungen anwenden
void applySettings() {
    const settings& s = settingsManager.getSettings();
    themeManager.setCurrentTheme(s.gfxTheme, ThemeType::GRAPHICS);
    themeManager.setCurrentTheme(s.sndTheme, ThemeType::SOUND);
    themeManager.setCurrentTheme(s.lvlTheme, ThemeType::LEVEL);
}
```

Diese Architektur bietet eine klare Trennung der Zuständigkeiten und erlaubt flexible Theme-Verwaltung. Die drei Manager könnten in einer eigenen Klasse wie `GameResources` oder `GameContext` gebündelt werden, um die Abhängigkeiten zentral zu verwalten.