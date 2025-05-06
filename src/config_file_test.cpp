#include "config_file.h"
#include <iostream>
#include <cstdlib>

int main() {
    if (ConfigFile config("."); config.init()) {
        std::cout << "Konfigurationspfade:\n"
                  << "Root: " << config.getProgramRoot() << "\n"
                  << "Config: " << config.getSettingsFile() << "\n"
                  << "Savegame: " << config.getSaveGameFile() << "\n"
                  << "Highscore: " << config.getHighScoreFile() << "\n"
                  << "Screenshots: " << config.getScreenshotDir() << "\n";
    } else {
        std::cout << "Initialisierung fehlgeschlagen\n";
    }

    return EXIT_SUCCESS;
}
