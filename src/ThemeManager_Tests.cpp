#include "ThemeManager.h"
#include "ConfigFileManager.h"
#include <iostream>
#include <cassert>
#include <chrono>
/*
* Testverzeichnisse mit verschiedenen Theme-Strukturen anlegen -> tests/themes
* Edge-Cases testen (fehlende Verzeichnisse, ungültige Themes)
* Theme-Vererbung und Fallbacks überprüfen
*/
void testThemeDetection(ThemeManager& manager) {
    std::cout << "\nTest ThemeDetection:\n";
    auto themes = manager.listThemes();

    std::cout << "Verfügbare Themes:\n";
    for (const auto& theme : themes) {
        std::cout << "- " << theme.name
                  << " (valid: " << (theme.valid ? "ja" : "nein")
                  << ", gfx: " << (theme.gfx ? "ja" : "nein")
                  << ", snd: " << (theme.snd ? "ja" : "nein")
                  << ", lvl: " << (theme.lvl ? "ja" : "nein") << ")\n";
    }
}

void testDefaultTheme(ThemeManager& manager) {
    std::cout << "\nTest DefaultTheme:\n";
    std::cout << "Standard-Theme: " << ThemeManager::getDefaultTheme() << "\n";
    std::cout << "Aktuelles Theme: " << manager.getCurrentTheme() << "\n";

    // Prüfe, ob das Standard-Theme "default" ist
    assert(ThemeManager::getDefaultTheme() == "default");
}

void testThemeChanging(ThemeManager& manager) {
    std::cout << "\nTest ThemeChanging:\n";

    // Aktuelles Theme merken
    std::string originalTheme = manager.getCurrentTheme();
    std::cout << "Original Theme: " << originalTheme << "\n";

    // Zu nicht existierendem Theme wechseln sollte fehlschlagen
    bool result = manager.setCurrentTheme("nicht_existierend");
    std::cout << "Wechsel zu nicht existierendem Theme: "
              << (result ? "erfolgreich (falsch!)" : "fehlgeschlagen (richtig)") << "\n";
    assert(!result);
    assert(manager.getCurrentTheme() == "default");

    // Zu einem existierenden Theme wechseln (falls vorhanden)
    auto themes = manager.listThemes();
    for (const auto& theme : themes) {
        if (theme.name != "default" && theme.valid) {
            result = manager.setCurrentTheme(theme.name);
            std::cout << "Wechsel zu " << theme.name << ": "
                      << (result ? "erfolgreich" : "fehlgeschlagen") << "\n";
            assert(result);
            assert(manager.getCurrentTheme() == theme.name);
            break;
        }
    }

    // Zurück zum Original-Theme
    manager.setCurrentTheme(originalTheme);
}

void testFilePathResolution(ThemeManager& manager) {
    std::cout << "\nTest FilePathResolution:\n";

    // Test mit verschiedenen Ressourcen
    std::string paths[] = {
        "gfx/paddle/base.png",
        "gfx/ball.png",
        "levels.txt",
        "nicht_existierende_datei.xyz"
    };

    for (const auto& path : paths) {
        std::string resolvedPath = manager.getThemeFilePath(path);
        std::cout << "Ressource '" << path << "' aufgelöst zu: "
                  << (resolvedPath.empty() ? "[nicht gefunden]" : resolvedPath) << "\n";

        // Ressource-Existenz prüfen
        bool exists = manager.themeHasResource(path);
        std::cout << "  Existiert: " << (exists ? "ja" : "nein") << "\n";
    }
}

void testThemePathCache(ThemeManager& manager) {
    std::cout << "\nTest ThemePathCache:\n";

    // Zeitmessung für wiederholte Anfragen
    auto start = std::chrono::high_resolution_clock::now();

    const int iterations = 1000;
    std::string path = "gfx/paddle/base.png";

    for (int i = 0; i < iterations; i++) {
        manager.getThemeFilePath(path);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << iterations << " Pfadauflösungen für '" << path << "': "
              << duration.count() << " ms\n";
    std::cout << "Durchschnittlich " << (duration.count() / iterations) << " ms pro Anfrage\n";
}

int main() {
    std::cout << "ThemeManager-Tests starten\n";

    // ConfigFileManager mit Test-Verzeichnis initialisieren
    ConfigFileManager config("./test");

    // ThemeManager initialisieren
    ThemeManager themeManager(config);

    // Tests ausführen
    testThemeDetection(themeManager);
    testDefaultTheme(themeManager);
    testThemeChanging(themeManager);
    testFilePathResolution(themeManager);
    testThemePathCache(themeManager);

    std::cout << "\nAlle Tests abgeschlossen.\n";
    return EXIT_SUCCESS;
}
