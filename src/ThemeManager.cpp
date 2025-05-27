#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <SDL2/SDL_log.h>

#include "ThemeManager.h"

ThemeManager::ThemeManager(const ConfigFileManager &configFileManager)
    : currentTheme(getDefaultTheme()), configFileManager(configFileManager) {
    scanThemes();
    pathCache.clear();
}

void ThemeManager::scanThemes() {
    themes.clear();
    const std::vector themeDirs = {
        configFileManager.getUserThemeDir(),
        ConfigFileManager::getGlobalThemeDir()
    };

    for (const auto &dir: themeDirs) {
        DIR *pdir = opendir(dir.c_str());
        if (pdir) {
            dirent *pent;
            while ((pent = readdir(pdir))) {
                std::string temp = pent->d_name;
                if (temp[0] != '.') {
                    std::string themePath = dir + "/" + temp;
                    struct stat st{};
                    if (stat(themePath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                        themeInfo ti;
                        ti.name = temp;
                        ti.valid = false;

                        // Prüfe auf gfx, snd und lvl
                        ti.gfx = (stat((themePath + "/gfx").c_str(), &st) == 0);
                        ti.snd = (stat((themePath + "/snd").c_str(), &st) == 0);
                        ti.lvl = (stat((themePath + "/levels.txt").c_str(), &st) == 0);
                        ti.valid = ti.gfx || ti.snd || ti.lvl;

                        themes.push_back(ti);
                    }
                }
            }
            closedir(pdir);
        }
    }
}

std::vector<themeInfo> ThemeManager::listThemes() {
    return themes;
}

bool ThemeManager::themeExists(const std::string &name) const {
    for (const auto &t: themes)
        if (t.name == name) return true;
    return false;
}

std::string ThemeManager::getDefaultTheme() {
    return "default";
}

// Aktuelles Theme setzen
bool ThemeManager::setCurrentTheme(const std::string &themeName) {
    if (themeExists(themeName)) {
        currentTheme = themeName;
        pathCache.clear(); // Cache leeren beim Theme-Wechsel
        return true;
    }
    // Bei ungültigem Theme auf Default zurücksetzen
    currentTheme = getDefaultTheme();
    return false;
}

// Aktuelles Theme abrufen
std::string ThemeManager::getCurrentTheme() const {
    return currentTheme;
}

// Überprüfen, ob eine Ressource in einem Theme existiert
bool ThemeManager::themeHasResource(const std::string &path, const std::string &theme) const {
    std::string themeToUse = theme.empty() ? currentTheme : theme;
    std::string filePath = getThemeFilePath(path, themeToUse);
    return !filePath.empty();
}

// getThemeFilePath anpassen für aktuelles Theme als Fallback
std::string ThemeManager::getThemeFilePath(const std::string &path, const std::string &theme) const {
    // Cache prüfen
    std::string cacheKey = (theme.empty() ? currentTheme : theme) + ":" + path;
    auto cacheIt = pathCache.find(cacheKey);
    if (cacheIt != pathCache.end()) {
        return cacheIt->second;
    }

    auto join = [](const std::string &a, const std::string &b) {
        if (a.empty()) return b;
        if (b.empty()) return a;
        if (a.back() == '/' && b.front() == '/')
            return a + b.substr(1);
        if (a.back() != '/' && b.front() != '/')
            return a + "/" + b;
        return a + b;
    };

    struct stat st{};
    std::string name;
    std::string themeToUse = theme.empty() ? currentTheme : theme;

    if (themeToUse != "default") {
        name = join(configFileManager.getUserThemeDir(), themeToUse + "/" + path);
        if (stat(name.c_str(), &st) == 0) {
            pathCache[cacheKey] = name;
            return name;
        }
        name = join(configFileManager.getGlobalThemeDir(), themeToUse + "/" + path);
        if (stat(name.c_str(), &st) == 0) {
            pathCache[cacheKey] = name;
            return name;
        }
    }
    name = join(configFileManager.getGlobalThemeDir(), "default/" + path);
    if (stat(name.c_str(), &st) == 0) {
        pathCache[cacheKey] = name;
        return name;
    }

    SDL_Log("File Error: Could not find '%s' in theme '%s'", path.c_str(), themeToUse.c_str());
    return "";
}
