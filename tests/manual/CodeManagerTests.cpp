// CodeManagerTests.cpp

#include <iostream>
#include "../../src/CodeManager.h"
#include "../../src/EventManager.h"
#include "../../src/ConfigFileManager.h"
#include "../../src/SettingsManager.h"
#include "../../src/ThemeManager.h"

void testThemeChange() {
    // Manager erstellen
    ConfigFileManager configFileManager;
    EventManager eventManager;
    SettingsManager settingsManager(configFileManager);
    ThemeManager themeManager(configFileManager);

    // CodeManager mit allen Abhängigkeiten erstellen
    CodeManager codeManager(eventManager, configFileManager, settingsManager, themeManager);
    codeManager.init();

    // Test 1: Theme-Wechsel auslösen
    std::cout << "Test 1: Theme-Wechsel mit validem Theme\n";
    ThemeData requestData;
    requestData.levelsTheme = {"default", "../themes/default"};
    eventManager.emit(GameEvent::LevelThemeRequested, requestData);

    // Test 2: Ungültiges Theme testen
    std::cout << "\nTest 2: Theme-Wechsel mit ungültigem Theme\n";
    requestData.levelsTheme = {"invalid", "../themes/invalid"};
    eventManager.emit(GameEvent::LevelThemeRequested, requestData);

    // Test 3: Theme aus Settings laden
    std::cout << "\nTest 3: Theme aus Settings laden\n";
    settingsManager.resetToDefaults();
    codeManager.init();
}

int main() {
    testThemeChange();
    return 0;
}
