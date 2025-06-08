#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "ConfigFileManager.h"

struct themeInfo {
    std::string name;
    bool snd, gfx, lvl, valid; // Valid bedeutet momentan, dass Daten vorhanden sind (kein leerer Ordner)
};

class ThemeManager {
public:
    explicit ThemeManager(const ConfigFileManager &configFileManager);

    std::vector<themeInfo> listThemes();

    bool themeExists(const std::string &name) const;

    std::string getThemeFilePath(const std::string &path, const std::string &theme = "") const;

    [[nodiscard]] static std::string getDefaultTheme();

    bool setCurrentTheme(const std::string &themeName);

    std::string getCurrentTheme() const;

    bool themeHasResource(const std::string &path, const std::string &theme = "") const;

    [[nodiscard]] const std::vector<std::string> &getThemeRoots() const { return themeRoots; }

private:
    std::vector<themeInfo> themes;
    std::string currentTheme;
    std::vector<std::string> themeRoots;
    const ConfigFileManager &configFileManager;
    mutable std::unordered_map<std::string, std::string> pathCache;

    void scanThemes();

    void initThemeRoots();
};
