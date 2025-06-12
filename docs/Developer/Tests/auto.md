# automatic tests

// Hilfsfunktion zum Überprüfen des Vorhandenseins und Erstellens von Testverzeichnissen
bool setupTestEnvironment(const std::string& basePath) {
// Überprüfe, ob Basis-Testverzeichnis existiert, sonst erstelle es
std::filesystem::path testDir = basePath;
if (!std::filesystem::exists(testDir)) {
if (!std::filesystem::create_directories(testDir)) {
SDL_Log("Konnte Testverzeichnis nicht erstellen: %s", testDir.c_str());
return false;
}
}

    // Erstelle notwendige Unterverzeichnisse für verschiedene Texturtypen
    std::vector<std::string> subdirs = {
        "gfx", "gfx/paddle", "gfx/ball", "gfx/brick", "gfx/powerup", 
        "gfx/effects", "gfx/title"
    };
    
    for (const auto& dir : subdirs) {
        std::filesystem::path subDir = testDir / dir;
        if (!std::filesystem::exists(subDir)) {
            if (!std::filesystem::create_directories(subDir)) {
                SDL_Log("Konnte Unterverzeichnis nicht erstellen: %s", subDir.c_str());
                return false;
            }
        }
    }
    return true;

}

// Hilfsfunktion zum Erstellen einer einfachen Test-Textur
bool createTestTexture(const std::string& path, int width = 64, int height = 64) {
SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
0xFF000000,
0x00FF0000,
0x0000FF00,
0x000000FF);
if (!surface) {
SDL_Log("Konnte keine Test-Surface erstellen: %s", SDL_GetError());
return false;
}

    // Einfaches Farbmuster erstellen
    SDL_Rect rect = {0, 0, width, height};
    SDL_FillRect(surface, &rect, SDL_MapRGBA(surface->format, 255, 0, 0, 255));

    // Eine diagonale Linie zeichnen
    for (int i = 0; i < width && i < height; i++) {
        ((Uint32*)surface->pixels)[i * surface->pitch/4 + i] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    }

    // Als PNG speichern
    if (IMG_SavePNG(surface, path.c_str()) != 0) {
        SDL_Log("Konnte Test-Textur nicht speichern: %s", IMG_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);
    return true;

}

// Hilfsfunktion zum Erstellen einer einfachen Eigenschaftsdatei
bool createTestProperties(const std::string& path, int cols = 1, int rows = 1) {
std::ofstream file(path);
if (!file.is_open()) {
SDL_Log("Konnte Eigenschaftsdatei nicht erstellen: %s", path.c_str());
return false;
}

    file << "cols=" << cols << std::endl;
    file << "rows=" << rows << std::endl;
    file << "ticks=100" << std::endl;
    file << "frames=" << (cols * rows) << std::endl;
    file << "bidir=0" << std::endl;
    file << "playing=0" << std::endl;
    file << "padding=0" << std::endl;
    file << "xoffset=1.0" << std::endl;
    file << "yoffset=1.0" << std::endl;
    file << "texcolor=FFFFFFFF" << std::endl;

    file.close();
    return true;

}

    // Einfache Test-Texturen erstellen
    createTestTexture(testThemePath + "/gfx/paddle/base.png");
    createTestTexture(testThemePath + "/gfx/paddle/glue.png");
    createTestTexture(testThemePath + "/gfx/paddle/gun.png");
    createTestTexture(testThemePath + "/gfx/ball/normal.png");
    createTestTexture(testThemePath + "/gfx/ball/fireball.png", 32, 32);
    createTestTexture(testThemePath + "/gfx/brick/base.png");
    createTestTexture(testThemePath + "/gfx/effects/particle.png", 16, 16);
    createTestTexture(testThemePath + "/gfx/effects/tail.png", 16, 16);
    createTestTexture(testThemePath + "/gfx/title/title.png", 256, 128);
    
    // Eigenschaftsdateien für einige Texturen erstellen
    createTestProperties(testThemePath + "/gfx/ball/normal.txt");
    createTestProperties(testThemePath + "/gfx/ball/fireball.txt", 4, 1); // Animation mit 4 Frames
    createTestProperties(testThemePath + "/gfx/effects/particle.txt");

    // Test 1: Theme setzen
    SDL_Log("Test 1: Theme setzen...");
    if (!textureManager.setSpriteTheme(testThemePath)) {
        SDL_Log("FEHLER: Konnte Theme nicht setzen");
    } else {
        SDL_Log("OK: Theme wurde erfolgreich gesetzt");
    }

    // Test 2: Einzelne Textur laden
    SDL_Log("Test 2: Einzelne Textur laden...");
    SpriteSheetAnimation* normalBall = textureManager.getBallTexture(BallTexture::Normal);
    if (!normalBall || normalBall->textureProperties.texture == 0) {
        SDL_Log("FEHLER: Konnte Ball-Textur nicht laden");
    } else {
        SDL_Log("OK: Ball-Textur wurde erfolgreich geladen (ID: %u)", 
                normalBall->textureProperties.texture);
    }

    // Test 3: Animation laden
    SDL_Log("Test 3: Animation laden...");
    SpriteSheetAnimation* fireballAnim = textureManager.getBallTexture(BallTexture::Fireball);
    if (!fireballAnim || fireballAnim->textureProperties.frames != 4) {
        SDL_Log("FEHLER: Animation wurde nicht korrekt geladen");
    } else {
        SDL_Log("OK: Animation wurde mit %d Frames geladen", 
                fireballAnim->textureProperties.frames);
    }

    // Test 4: Nicht existierende Textur laden
    SDL_Log("Test 4: Nicht existierende Textur laden...");
    SpriteSheetAnimation* nonExistent = textureManager.getTexture("gfx/nonexistent");
    if (nonExistent) {
        SDL_Log("FEHLER: Nicht existierende Textur wurde fälschlicherweise geladen");
    } else {
        SDL_Log("OK: Nicht existierende Textur wurde korrekt als nicht vorhanden erkannt");
    }

    // Test 5: Cache testen (zweimal dieselbe Textur laden)
    SDL_Log("Test 5: Cache testen...");
    SpriteSheetAnimation* firstLoad = textureManager.getTexture("gfx/paddle/base");
    SpriteSheetAnimation* secondLoad = textureManager.getTexture("gfx/paddle/base");
    if (firstLoad != secondLoad) {
        SDL_Log("FEHLER: Cache funktioniert nicht korrekt");
    } else {
        SDL_Log("OK: Cache funktioniert korrekt");
    }

    // Test 6: Enum-basierter Zugriff
    SDL_Log("Test 6: Enum-basierten Zugriff testen...");
    SpriteSheetAnimation* paddleTexture = textureManager.getPaddleTexture(PaddleTexture::Base);
    if (!paddleTexture || paddleTexture->textureProperties.texture == 0) {
        SDL_Log("FEHLER: Enum-basierter Zugriff fehlgeschlagen");
    } else {
        SDL_Log("OK: Enum-basierter Zugriff funktioniert");
    }

    // Test 7: Theme löschen
    SDL_Log("Test 7: Theme löschen...");
    textureManager.clearTheme();
    SpriteSheetAnimation* afterClear = textureManager.getBallTexture(BallTexture::Normal);
    if (afterClear) {
        SDL_Log("FEHLER: Theme wurde nicht richtig gelöscht");
    } else {
        SDL_Log("OK: Theme wurde erfolgreich gelöscht");
    }

    // Test 8: Neues Theme laden
    SDL_Log("Test 8: Neues Theme laden...");
    if (!textureManager.setSpriteTheme(testThemePath)) {
        SDL_Log("FEHLER: Konnte Theme nicht neu laden");
    } else {
        SDL_Log("OK: Theme wurde erfolgreich neu geladen");
        SpriteSheetAnimation* reloaded = textureManager.getBallTexture(BallTexture::Normal);
        if (!reloaded) {
            SDL_Log("FEHLER: Konnte Textur nach Neu-Laden nicht abrufen");
        } else {
            SDL_Log("OK: Textur nach Neu-Laden erfolgreich abgerufen");
        }
    }

// Test 9: Bildformatunterstützung testen
SDL_Log("Test 9: Bildformatunterstützung testen...");
if (!textureManager.isImageFormatSupported("png")) {
SDL_Log("FEHLER: PNG-Format wird nicht unterstützt");
} else {
SDL_Log("OK: PNG-Format wird unterstützt");
}

    if (!textureManager.isImageFormatSupported("jpg")) {
        SDL_Log("FEHLER: JPG-Format wird nicht unterstützt");
    } else {
        SDL_Log("OK: JPG-Format wird unterstützt");
    }

    if (textureManager.isImageFormatSupported("gif")) {
        SDL_Log("FEHLER: GIF-Format sollte nicht unterstützt werden");
    } else {
        SDL_Log("OK: GIF-Format wird korrekt als nicht unterstützt erkannt");
    }

    return true;

// Test 10: Fehlerhafte Textur laden
SDL_Log("Test 10: Fehlerhafte Textur laden...");
SpriteSheetAnimation* invalidTexture = textureManager.getTexture("gfx/invalid");
if (invalidTexture) {
SDL_Log("FEHLER: Fehlerhafte Textur wurde fälschlicherweise geladen");
} else {
SDL_Log("OK: Fehlerhafte Textur wurde korrekt als nicht vorhanden erkannt");
}

    // Test 11: Mehrere Themes nacheinander laden
    SDL_Log("Test 11: Mehrere Themes nacheinander laden...");
    if (!textureManager.setSpriteTheme(testThemePath)) {
        SDL_Log("FEHLER: Konnte erstes Theme nicht setzen");
    } else {
        SDL_Log("OK: Erstes Theme erfolgreich gesetzt");
    }

    if (!textureManager.setSpriteTheme(testThemePath)) {
        SDL_Log("FEHLER: Konnte zweites Theme nicht setzen");
    } else {
        SDL_Log("OK: Zweites Theme erfolgreich gesetzt");
    }

    return true;

}

    // Aufräumen

void cleanupTestEnvironment(const std::string& basePath) {
std::filesystem::path testDir = basePath;
if (std::filesystem::exists(testDir)) {
std::filesystem::remove_all(testDir);
SDL_Log("Testverzeichnis %s wurde aufgeräumt", testDir.c_str());
} else {
SDL_Log("Testverzeichnis %s existiert nicht, kein Aufräumen nötig", testDir.c_str());
}
}