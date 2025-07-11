#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <SDL2/SDL_log.h>

#include "ThemeManager.h"

ThemeManager::ThemeManager(const ConfigFileManager &configFileManager)
    : currentTheme(getDefaultTheme()), configFileManager(configFileManager) {
    initThemeRoots();
    scanThemes();
    pathCache.clear();
}

void ThemeManager::scanThemes() {
    themes.clear();

    for (const auto &rootDir: themeRoots) {
        if (DIR *pdir = opendir(rootDir.c_str())) {
            dirent *pent;
            while ((pent = readdir(pdir))) {
                if (std::string temp = pent->d_name; temp[0] != '.') {
                    std::string themePath = rootDir;
                    themePath.append("/");
                    themePath.append(temp);
                    struct stat st{};
                    if (stat(themePath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                        themeInfo ti;
                        ti.name = temp;
                        ti.valid = false;

                        // Check SubThemes
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
    return std::ranges::any_of(themes,
                               [&name](const auto &t) { return t.name == name; });
}

std::string ThemeManager::getDefaultTheme() {
    return "default";
}

bool ThemeManager::setCurrentTheme(const std::string &themeName) {
    if (themeExists(themeName)) {
        currentTheme = themeName;
        pathCache.clear();
        return true;
    }
    // Bei ungültigem Theme auf Default zurücksetzen
    currentTheme = getDefaultTheme();
    return false;
}

std::string ThemeManager::getCurrentTheme() const {
    return currentTheme;
}

// Überprüfen, ob eine Ressource in einem Theme existiert
bool ThemeManager::themeHasResource(const std::string &path, const std::string &theme) const {
    const std::string themeToUse = theme.empty() ? currentTheme : theme;
    const std::string filePath = getThemeFilePath(path, themeToUse);
    return !filePath.empty();
}

// getThemeFilePath anpassen für aktuelles Theme als Fallback
std::string ThemeManager::getThemeFilePath(const std::string &path, const std::string &theme) const {
    // Cache prüfen
    const std::string cacheKey = (theme.empty() ? currentTheme : theme) + ":" + path;
    if (const auto cacheIt = pathCache.find(cacheKey); cacheIt != pathCache.end()) {
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
    const std::string themeToUse = theme.empty() ? currentTheme : theme;

    if (themeToUse != "default") {
        name = join(configFileManager.getUserThemeDir(), themeToUse + "/" + path);
        if (stat(name.c_str(), &st) == 0) {
            pathCache[cacheKey] = name;
            return name;
        }
        name = join(ConfigFileManager::getGlobalThemeDir(), themeToUse + "/" + path);
        if (stat(name.c_str(), &st) == 0) {
            pathCache[cacheKey] = name;
            return name;
        }
    }
    name = join(ConfigFileManager::getGlobalThemeDir(), "default/" + path);
    if (stat(name.c_str(), &st) == 0) {
        pathCache[cacheKey] = name;
        return name;
    }

    SDL_Log("File Error: Could not find '%s' in theme '%s'", path.c_str(), themeToUse.c_str());
    return "";
}

void ThemeManager::initThemeRoots() {
    themeRoots.clear();

    // Root-Verzeichnis (aus Constructor)
    themeRoots.push_back(configFileManager.getProgramRoot() + "/themes");

    // XDG_CONFIG_HOME
    if (const char *configHome = getenv("XDG_CONFIG_HOME")) {
        themeRoots.push_back(std::string(configHome) + "/sdl-ball/themes");
    } else if (const char *home = getenv("HOME")) {
        themeRoots.push_back(std::string(home) + "/.config/sdl-ball/themes");
    }

    // XDG_DATA_HOME
    if (const char *dataHome = getenv("XDG_DATA_HOME")) {
        themeRoots.push_back(std::string(dataHome) + "/sdl-ball/themes");
    } else if (const char *home = getenv("HOME")) {
        themeRoots.push_back(std::string(home) + "/.local/share/sdl-ball/themes");
    }

    // System-weites Verzeichnis
    themeRoots.emplace_back("/usr/share/sdl-ball/themes");

    // Build-Verzeichnis (für Entwicklung)
    themeRoots.push_back(THEME_DIRECTORY);

    // Ungültige Verzeichnisse entfernen
    std::erase_if(themeRoots,
                  [](const std::string &dir) {
                      return !ConfigFileManager::checkDir(dir);
                  });
}
