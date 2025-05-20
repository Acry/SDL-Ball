#include "ConfigFileManager.h"
#include <iostream>
#include <cassert>

#include "SettingsManager.h"

void testSettingsFile(const ConfigFileManager &config) {
    std::cout << "\nTest SettingsFile:\n";
    std::cout << "Settings file path: " << config.getSettingsFile() << "\n";
}

int main() {
    ConfigFileManager config("./test");
    // was macht der Konstruktor?
    SettingsManager settings(config);
    // set defaults - .defaultSettings
    // init: loadSettings, validateSettings, resetToLoaded
    testSettingsFile(config);

    return EXIT_SUCCESS;
}