#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "ConfigFileManager.h"

struct themeInfo {
    std::string name;
    bool snd, gfx, lvl, valid; // Valid bedeutet, dass Daten vorhanden sind (kein leerer Ordner)
};

class ThemeManager {
public:
    explicit ThemeManager(const ConfigFileManager &configFileManager);

    // Bestehende Methoden
    std::vector<themeInfo> listThemes();

    bool themeExists(const std::string &name) const;

    std::string getThemeFilePath(const std::string &path, const std::string &theme = "") const;

    [[nodiscard]] static std::string getDefaultTheme();

    // Neue Methoden
    bool setCurrentTheme(const std::string &themeName);

    std::string getCurrentTheme() const;

    bool themeHasResource(const std::string &path, const std::string &theme = "") const;

private:
    std::vector<themeInfo> themes;
    std::string currentTheme;
    const ConfigFileManager &configFileManager;
    mutable std::unordered_map<std::string, std::string> pathCache;

    void scanThemes();
};
