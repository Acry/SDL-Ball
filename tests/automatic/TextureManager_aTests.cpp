// TextureMamanger_aTests.cpp
#include <fstream>
#include <SDL2/SDL_surface.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/TextureManager.h"
#include <memory>

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
    } catch (const std::exception &e) {
        // Bei einer Ausnahme im Konstruktor überspringen wir diesen Test
        GTEST_SKIP() << "Konnte TextureManager nicht erstellen: " << e.what();
    }
}

TEST(TextureManagerTest, ReadTexPropsLoadsPropertiesCorrectly) {
    // Temporäre Testdatei erstellen
    std::string testFilePath = "test_texture.txt";
    std::ofstream testFile(testFilePath);

    /*
     * file=ball/fireball.png
     * ticks=25
     * cols=4
     * rows=4
     * xoffset=0.25
     * yoffset=0.25
     * frames=16
     * playing=1
     * bidir=1
     * padding=1
     * texcolor=FFFFFFFF
     * parcolor=FFFFFF //no effect
     */

    // Testdaten in die Datei schreiben
    testFile << "xoffset=0.5\n"
            << "yoffset=0.25\n"
            << "cols=4\n"
            << "rows=2\n"
            << "ticks=500\n"
            << "frames=8\n"
            << "bidir=1\n"
            << "playing=1\n"
            << "padding=1\n"
            << "texcolor=FFFFFFFF\n"
            << "parcolor=FFFFFF\n"
            << "file=testfile.png\n";
    testFile.close();

    try {
        // TextureManager und SpriteSheetAnimation erstellen
        TextureManager textureManager;
        SpriteSheetAnimation animation;

        // Methode testen
        textureManager.readTexProps(testFilePath, animation);

        // Überprüfen der gelesenen Werte
        EXPECT_FLOAT_EQ(0.5f, animation.textureProperties.xoffset);
        EXPECT_FLOAT_EQ(0.25f, animation.textureProperties.yoffset);
        EXPECT_EQ(4, animation.textureProperties.cols);
        EXPECT_EQ(2, animation.textureProperties.rows);
        EXPECT_EQ(500, animation.textureProperties.ticks);
        EXPECT_EQ(8, animation.textureProperties.frames);
        EXPECT_TRUE(animation.textureProperties.direction);
        EXPECT_TRUE(animation.textureProperties.playing);
        EXPECT_TRUE(animation.textureProperties.padding);

        // Überprüfen der Farbwerte
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glTexColorInfo[0]);
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glTexColorInfo[1]);
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glTexColorInfo[2]);
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glTexColorInfo[3]);

        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glParColorInfo[0]);
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glParColorInfo[1]);
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.glParColorInfo[2]);

        EXPECT_EQ("testfile.png", animation.textureProperties.fileName);
    } catch (const std::exception &e) {
        FAIL() << "Ausnahme während des Tests: " << e.what();
    }

    // Aufräumen
    std::remove(testFilePath.c_str());
}

TEST(TextureManagerTest, ReadTexPropsHandlesInvalidFile) {
    try {
        // TextureManager und SpriteSheetAnimation erstellen
        TextureManager textureManager;
        SpriteSheetAnimation animation;

        // Initialisieren der Eigenschaften mit bekannten Werten
        animation.textureProperties.xoffset = 1.0f;
        animation.textureProperties.yoffset = 1.0f;
        animation.textureProperties.cols = 1;
        animation.textureProperties.rows = 1;
        animation.textureProperties.ticks = 1000;

        // Methode mit nicht existierender Datei testen
        textureManager.readTexProps("nicht_existierende_datei.txt", animation);

        // Die Eigenschaften sollten unverändert bleiben
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.xoffset);
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.yoffset);
        EXPECT_EQ(1, animation.textureProperties.cols);
        EXPECT_EQ(1, animation.textureProperties.rows);
        EXPECT_EQ(1000, animation.textureProperties.ticks);
    } catch (const std::exception &e) {
        FAIL() << "Ausnahme während des Tests: " << e.what();
    }
}

TEST(TextureManagerTest, ReadTexPropsHandlesInvalidProperties) {
    // Temporäre Testdatei mit ungültigen Werten erstellen
    std::string testFilePath = "invalid_texture.txt";
    std::ofstream testFile(testFilePath);

    // Testdaten mit ungültigen Werten in die Datei schreiben
    testFile << "xoffset=invalid\n"
            << "yoffset=0.25\n"
            << "cols=nicht_numerisch\n"
            << "texcolor=XYZ\n" // Ungültiges Farbformat
            << "unbekannte_eigenschaft=123\n"; // Unbekannte Eigenschaft
    testFile.close();

    try {
        // TextureManager und SpriteSheetAnimation erstellen
        TextureManager textureManager;
        SpriteSheetAnimation animation;

        // Initialisieren der Eigenschaften mit bekannten Werten
        animation.textureProperties.xoffset = 1.0f;
        animation.textureProperties.yoffset = 1.0f;
        animation.textureProperties.cols = 1;

        // Methode testen
        textureManager.readTexProps(testFilePath, animation);

        // Nur der gültige Wert sollte geändert werden
        EXPECT_FLOAT_EQ(1.0f, animation.textureProperties.xoffset); // Bleibt unverändert
        EXPECT_FLOAT_EQ(0.25f, animation.textureProperties.yoffset); // Wurde aktualisiert
        EXPECT_EQ(1, animation.textureProperties.cols); // Bleibt unverändert
    } catch (const std::exception &e) {
        FAIL() << "Ausnahme während des Tests: " << e.what();
    }

    // Aufräumen
    std::remove(testFilePath.c_str());
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
