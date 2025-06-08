#include "ConfigFileManager.h"

#include <algorithm>

#include "config.h"
#include <sys/stat.h>
#include <cstdlib>
#include <utility>

ConfigFileManager::ConfigFileManager(std::string root) : programRoot(std::move(root)) {
    init();
}

bool ConfigFileManager::init() {
    // XDG oder Home Verzeichnis ermitteln
    if (programRoot.empty()) {
        if (getenv("XDG_CONFIG_HOME") != nullptr) {
            programRoot = getenv("XDG_CONFIG_HOME");
        } else if (getenv("HOME") != nullptr) {
            programRoot = getenv("HOME");
            programRoot.append("/.config");
        }
    }

    // Wenn keine Rechte für ~/.config, auf ./ ausweichen
    if (programRoot.empty() || !checkDir(programRoot)) {
        programRoot = "./";
    }

    // Game-Verzeichnis erstellen
    std::string gameDir = programRoot + "/" GAME_NAME;
    if (!checkDir(gameDir)) {
        programRoot = "./";
        gameDir = "./";
    }
    programRoot = gameDir;

    // Pfade setzen
    settingsFile = programRoot + "/" CONFIG_FILE;
    saveGameFile = programRoot + "/" SAVEGAME_FILE;
    highScoreFile = programRoot + "/" HIGHSCORE_FILE;

    // Screenshot-Verzeichnis explizit anlegen.
    // Erste Priorität: Im Config-Verzeichnis
    screenshotDir = programRoot + "/" SCREENSHOT_DIR;
    if (!checkDir(screenshotDir)) {
        // Zweite Priorität: Lokales Verzeichnis
        screenshotDir = "./" SCREENSHOT_DIR;
        if (!checkDir(screenshotDir)) {
            // Letzte Option: Aktuelles Verzeichnis
            screenshotDir = "./";
        }
    }

    return true;
}

bool ConfigFileManager::checkDir(const std::string &dir) {
    struct stat st{};

    // Prüft ob Verzeichnis existiert (stat == 0 bedeutet existiert)
    if (stat(dir.c_str(), &st) != 0) {
        // Existiert nicht -> erstelle es
        // Rechte: rwx für User und Gruppe (0770)
        return mkdir(dir.c_str(), S_IRWXU | S_IRWXG) == 0;
    }
    // Existiert bereits
    return true;
}

std::string ConfigFileManager::getUserThemeDir() const {
    return programRoot + "themes";
}

std::string ConfigFileManager::getGlobalThemeDir() {
    return DATADIR "themes";
}

