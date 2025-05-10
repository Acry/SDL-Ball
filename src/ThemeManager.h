// ThemeManager.h
#pragma once
#include <string>
#include <vector>

#include "ConfigFileManager.h"

struct themeInfo {
    std::string name;
    bool snd, gfx, lvl, valid; // Valid means that there seems to be data of some kind (ie. not just an empty folder)
};

class ThemeManager {
public:
    explicit ThemeManager(const ConfigFileManager& configFile);

    std::vector<themeInfo> listThemes();
    bool themeExists(const std::string& name) const;
    std::string getThemeFilePath(const std::string& path, const std::string& theme) const;
    [[nodiscard]] static std::string getDefaultTheme();

private:
    std::vector<themeInfo> themes;
    const ConfigFileManager& configFile;
    void scanThemes();
};
