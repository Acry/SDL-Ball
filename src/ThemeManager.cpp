// ThemeManager.cpp
#include "ThemeManager.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <SDL2/SDL_log.h>

ThemeManager::ThemeManager(const ConfigFileManager &configFile) : configFile(configFile) {
    scanThemes();
}

void ThemeManager::scanThemes() {
    themes.clear();
    const std::vector themeDirs = {
        configFile.getUserThemeDir(),
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
                        ti.gfx = (stat((themePath + "gfx").c_str(), &st) == 0);
                        ti.snd = (stat((themePath + "snd").c_str(), &st) == 0);
                        ti.lvl = (stat((themePath + "levels.txt").c_str(), &st) == 0);
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

std::string ThemeManager::getThemeFilePath(const std::string &path, const std::string &theme) const {
    auto join = [](const std::string& a, const std::string& b) {
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

    if (theme != "default") {
        name = join(configFile.getUserThemeDir(), theme + "/" + path);
        if (stat(name.c_str(), &st) == 0) return name;
        name = join(configFile.getGlobalThemeDir(), theme + "/" + path);
        if (stat(name.c_str(), &st) == 0) return name;
    }
    name = join(configFile.getGlobalThemeDir(), "default/" + path);
    if (stat(name.c_str(), &st) == 0) return name;
    SDL_Log("File Error: Could not find '%s'", path.c_str());
    return name;
}
