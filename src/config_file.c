#include "config.h"
#include "config_file.h"
#include <sys/stat.h>
#include <cstdlib>

Config::Config(const std::string& root) :
    programRoot(root),
    saveGameFile(""),
    settingsFile(""),
    highScoreFile(""),
    screenshotDir("")
{
}

bool Config::init() {
    if (programRoot.empty()) {
        if (getenv("XDG_CONFIG_HOME") != NULL) {
            programRoot = getenv("XDG_CONFIG_HOME");
        } else if (getenv("HOME") != NULL) {
            programRoot = getenv("HOME");
            programRoot.append("/.config");
        }
    }

    if(programRoot.empty()) {
        return false;
    }

    if(!checkDir(programRoot)) {
        return false;
    }

    programRoot.append("/" GAME_NAME);

    if(!checkDir(programRoot)) {
        return false;
    }

    saveGameFile = programRoot + SAVEGAME_PATH;
    settingsFile = programRoot + CONFIG_PATH;
    highScoreFile = programRoot + HIGHSCORE_PATH;
    screenshotDir = programRoot + SCREENSHOT_PATH;

    if(!checkDir(screenshotDir)) {
        screenshotDir = "./";
    }

    return true;
}

bool Config::checkDir(const std::string& dir) {
    struct stat st;

    if(stat(dir.c_str(), &st) != 0) {
        return mkdir(dir.c_str(), S_IRWXU | S_IRWXG) == 0;
    }
    return true;
}