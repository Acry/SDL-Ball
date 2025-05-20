#include "ConfigFileManager.h"
#include <iostream>
#include <cassert>

void testConfigPaths(const ConfigFileManager &config) {
    std::cout << "Test Konfigurationspfade:\n"
            << "Root: " << config.getProgramRoot() << "\n"
            << "Config: " << config.getSettingsFile() << "\n"
            << "Savegame: " << config.getSaveGameFile() << "\n"
            << "Highscore: " << config.getHighScoreFile() << "\n"
            << "Screenshots: " << config.getScreenshotDir() << "\n";
}

void testSettingsFile(const ConfigFileManager &config) {
    std::cout << "\nTest SettingsFile:\n";
    std::cout << "Settings file path: " << config.getSettingsFile() << "\n";
}

int main() {
    const ConfigFileManager config("./test");

    testConfigPaths(config);
    testSettingsFile(config);

    return EXIT_SUCCESS;
}
