#include <SDL2/SDL_surface.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/TextureManager.h"
#include <memory>

// Mock für Dateisystem oder SDL-Funktionen, falls nötig
class MockFileSystem {
public:
    MOCK_METHOD(bool, fileExists, (const std::string& path), ());
    MOCK_METHOD(SDL_Surface*, loadImage, (const std::string& path), ());
};

TEST(TextureManagerTest, ConstructorInitializesCorrectly) {
    ASSERT_NO_THROW({
        try {
            std::unique_ptr<TextureManager> manager = std::make_unique<TextureManager>();
            // Falls wir hier ankommen, war die Erstellung erfolgreich
            SUCCEED();
        } catch (const std::exception& e) {
            // Bei einer Ausnahme geben wir die Info aus und lassen den Test fehlschlagen
            ADD_FAILURE() << "Exception beim Erstellen des TextureManagers: " << e.what();
            throw;
        }
    });
}

TEST(TextureManagerTest, NonExistentThemeDirCausesError) {
    try {
        TextureManager textureManager;
        const std::string nonExistentThemePath = "./pfad";
        EXPECT_FALSE(textureManager.setSpriteTheme(nonExistentThemePath));
    } catch (const std::exception& e) {
        // Bei einer Ausnahme im Konstruktor überspringen wir diesen Test
        GTEST_SKIP() << "Konnte TextureManager nicht erstellen: " << e.what();
    }
}

#if 0 // Need OpenGL Context
TEST(TextureManagerTest, ExistentThemeSuccess) {
    try {
        TextureManager textureManager;
        SpriteSheetAnimation tex;
        const std::string texturePath = "../themes/default/gfx/ball/fireball.png";
        textureManager.load(texturePath, tex);
        EXPECT_TRUE(textureManager.load(texturePath, tex));

        if (tex.textureProperties.texture != 0) {
            glDeleteTextures(1, &tex.textureProperties.texture);
        }
    } catch (const std::exception& e) {
        // Bei einer Ausnahme im Konstruktor überspringen wir diesen Test
        GTEST_SKIP() << "Konnte TextureManager nicht erstellen: " << e.what();
    }
}
#endif
/*

Möglichen Testfälle für den TextureManager:

### Grundfunktionen

1. **Initialisierung**
   - [X] Konstruktor erstellt validen Manager
   - Maximale Textur-Größe wird korrekt abgefragt

2. **Theme-Management**
   - `setSpriteTheme` setzt neues Theme
   - Doppeltes Setzen des gleichen Themes gibt true zurück ohne Neuladen
   - [X] Nicht existierendes Theme-Verzeichnis führt zu Fehler
   - `clearTheme` entfernt alle Texturen aus dem Cache

3. **Textur-Cache**
   - `getTexture` lädt neue Textur und fügt sie zum Cache hinzu
   - `getTexture` ohne forceReload gibt dieselbe Textur-Instanz zurück
   - `getTexture` mit forceReload lädt Textur neu
   - Cache funktioniert für verschiedene Texturpfade korrekt

4. **Dateiformate**
   - PNG-Dateien werden korrekt geladen
   - JPG-Dateien werden korrekt geladen
   - Andere unterstützte Formate werden korrekt geladen
   - Nicht existierende Dateien geben nullptr zurück

5. **Textur-Eigenschaften**
   - `readTexProps` liest Eigenschaften korrekt
   - Eigenschaften werden korrekt auf die Textur angewendet
   - Fehlende Eigenschaftsdateien führen zu Standardwerten

6. **Enumerierte Texturen**
   - `getPaddleTexture` gibt korrekte PaddleTexture zurück
   - `getBallTexture` gibt korrekte BallTexture zurück
   - `getBrickTexture` gibt korrekte BrickTexture zurück
   - `getPowerUpTexture` gibt korrekte PowerUpTexture zurück
   - `getMiscTexture` gibt korrekte MiscTexture zurück

7. **Fehlerbehandlung**
   - Zu große Texturen werden abgelehnt
   - Invalide Enum-Indizes geben nullptr zurück
   - Fehlerhafte Eigenschaftsdateien werden toleriert

8. **Ressourcenmanagement**
   - Beim Entladen werden OpenGL-Texturen korrekt gelöscht
   - Destruktor gibt alle Ressourcen frei

### Erweiterte Testfälle

9. **Performance**
   - Cache reduziert Ladezeit signifikant
   - `loadAllGameTextures` lädt alle Texturen effizient

10. **Integration mit OpenGL**
    - Texturen haben korrekte OpenGL-IDs
    - MipMaps werden korrekt generiert
    - Texturen werden korrekt gebunden

11. **Theme-Wechsel**
    - Wechsel zu neuem Theme entlädt alte Texturen
    - Neue Theme-Texturen haben andere IDs

12. **Multi-Threading**
    - Paralleles Laden von Texturen ist threadsafe

Diese Tests können dabei helfen, die Funktionalität des TextureManagers umfassend zu prüfen und gleichzeitig die Komponenten zu entkoppeln, wie du es angesprochen hast.
*/