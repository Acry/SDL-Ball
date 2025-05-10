#include "ConfigFileManager.h"
#include "settings_manager.h"
#include <iostream>
#include <cassert>

#include "config.h"

void testConfigPaths(const ConfigFileManager &config) {
    std::cout << "Test Konfigurationspfade:\n"
            << "Root: " << config.getProgramRoot() << "\n"
            << "Config: " << config.getSettingsFile() << "\n"
            << "Savegame: " << config.getSaveGameFile() << "\n"
            << "Highscore: " << config.getHighScoreFile() << "\n"
            << "Screenshots: " << config.getScreenshotDir() << "\n";
}

void testSettingsManager(ConfigFileManager &config) {
    std::cout << "\nTest SettingsManager:\n";

    // Debug-Ausgabe
    std::cout << "Settings file path: " << config.getSettingsFile() << "\n";

    // Sicherstellen dass beide init() aufgerufen werden


    SettingsManager sm(config);
    assert(sm.init()); // Settings Manager initialisieren

    // Test Defaultwerte
    const auto &settings = sm.getSettings();
    assert(settings.fps == DEFAULT_FPS);
    assert(settings.fullscreen == DEFAULT_FULLSCREEN);

    // Test Speichern
    auto &mutableSettings = sm.getSettings();
    mutableSettings.fps = 60;
    assert(sm.save());

    std::cout << "Alle Tests erfolgreich!\n";
}

int main() {
    ConfigFileManager config("./test"); // Explizites Testverzeichnis
    assert(config.init()); // Sicherstellen dass init erfolgreich war

    testConfigPaths(config);
    testSettingsManager(config);

    return EXIT_SUCCESS;
}
