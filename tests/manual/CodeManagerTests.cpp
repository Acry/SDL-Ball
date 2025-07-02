// CodeManagerTests.cpp
#include <iostream>
#include "../../src/CodeManager.h"
#include "../../src/EventManager.h"
#include "../../src/ConfigFileManager.h"
#include "../../src/SettingsManager.h"
#include "../../src/ThemeManager.h"

void testThemeChange() {
    ConfigFileManager configFileManager("./test_config"); // Testverzeichnis
    EventManager eventManager;
    SettingsManager settingsManager(configFileManager);
    ThemeManager themeManager(configFileManager);

    CodeManager codeManager;

    // Event-Listener für Theme-Änderungen
    eventManager.addListener(GameEvent::LevelThemeChanged,
                             [](const ThemeData &data) {
                                 std::cout << "Theme geändert:\n"
                                         << "Sprites: " << data.spritesTheme.subThemePath << "\n"
                                         << "Fonts: " << data.fontsTheme.subThemePath << "\n"
                                         << "Levels: " << data.levelsTheme.subThemePath << "\n"
                                         << "Background: " << data.backgroundTheme.subThemePath << "\n"
                                         << "Sound: " << data.soundTheme.subThemePath << "\n";
                             }, nullptr);

    // Test 1: Standard-Theme
    std::cout << "Test 1: Standard-Theme laden\n";
    ThemeData requestData;
    requestData.levelsTheme = {"default", configFileManager.getGlobalThemeDir() + "/default"};
    eventManager.emit(GameEvent::LevelThemeRequested, requestData);

    // Test 2: Alternatives Theme
    std::cout << "\nTest 2: Alternatives Theme laden\n";
    requestData.levelsTheme = {"dio-sound-theme", configFileManager.getGlobalThemeDir() + "/dio-sound-theme"};
    eventManager.emit(GameEvent::LevelThemeRequested, requestData);
}

int main() {
    testThemeChange();
    return 0;
}
