// TextManager.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <filesystem>
#include <utility>
#include <map>
#include <set>

#include "TextManager.h"
#include "TextureUtilities.h"

#define DEBUG_ATLAS 0

TextManager::TextManager() : fontInfo{} {
    for (auto &i: fontInfo) {
        i.texture = 0;
        for (Uint32 j = 0; j < CHARS; j++) {
            i.uv[j] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        }
    }
    for (auto &i: fontInfo) {
        i.texture = 0;
    }
    TTF_Init();
}

bool isUtf8StartByte(unsigned char byte) {
    // 0xC0 = 11000000, prüft auf führendes Bit-Muster 11xxxxxx
    return (byte & 0xC0) == 0xC0;
}

int getUtf8ByteCount(unsigned char byte) {
    if ((byte & 0x80) == 0) return 1; // 0xxxxxxx - ASCII (1 Byte)
    if ((byte & 0xE0) == 0xC0) return 2; // 110xxxxx - 2 Bytes (z.B. ö)
    if ((byte & 0xF0) == 0xE0) return 3; // 1110xxxx - 3 Bytes
    if ((byte & 0xF8) == 0xF0) return 4; // 11110xxx - 4 Bytes
    return 0; // Ungültiges UTF-8-Byte
}

bool TextManager::genFontTex(const std::string &TTFfontName, const int fontSize, const Fonts font) {
    const std::string fullPath = currentTheme + "/" + "font/" + TTFfontName;

    TTF_Font *ttfFont = nullptr;
    ttfFont = TTF_OpenFont(fullPath.c_str(), fontSize);
    if (!ttfFont) {
        SDL_Log("TTF_OpenFont: %s", TTF_GetError());
        return false;
    }

    char tempChar[2] = {0, 0};
    int characterWidth = 0, characterHeight = 0; // Size of the rendered character in Pixels
    SDL_Rect src = {0, 0, 0, 0}, dst = {0, 0, 0, 0};
    const int fontIndex = static_cast<int>(font);

    TTF_SetFontHinting(ttfFont, TTF_HINTING_LIGHT_SUBPIXEL);

    const int ascent = TTF_FontAscent(ttfFont); // Obere Höhe über der Baseline
    const int descent = TTF_FontDescent(ttfFont); // Untere Höhe unter der Baseline
    const int lineSkip = TTF_FontLineSkip(ttfFont); // Empfohlener Zeilenabstand

    constexpr int surfaceDim = 1024; // Size of the surface to render the font to
    SDL_Surface *targetSurface = SDL_CreateRGBSurfaceWithFormat(0, surfaceDim, surfaceDim, 32, SDL_PIXELFORMAT_RGBA32);

    dst.x = 0;
    dst.y = 0;

    fontInfo[fontIndex].height = 0.0f;
    float highestCharacter = 0.0f;
    // ASCII printable characters (character code 32-127) - see man ascii
    for (Uint32 i = 32; i < CHARS; i++) {
        constexpr float referenceWidth = 1024.0f; // Reference width for normalization
        constexpr SDL_Color white = {255, 255, 255, 255};
        tempChar[0] = static_cast<char>(i);

        // Render character to surface
        SDL_Surface *character = TTF_RenderText_Blended(ttfFont, tempChar, white);
        SDL_SetSurfaceAlphaMod(character, 0xFF);
        TTF_SizeText(ttfFont, tempChar, &characterWidth, &characterHeight);

        // Pixel-Höhe für den größten Buchstaben merken
        if (characterHeight > highestCharacter) {
            highestCharacter = static_cast<float>(characterHeight);
        }

        src.x = 0;
        src.y = 0;
        src.w = characterWidth;
        src.h = characterHeight;

        if (dst.x + characterWidth > surfaceDim) {
            dst.x = 1;
            dst.y += characterHeight + 2;
            if (dst.y > surfaceDim) {
                SDL_Log("Too many chars for tex (%d)", i);
            }
        }

        // Neue Berechnung (OpenGL-konform mit invertierten v-Koordinaten):
        fontInfo[fontIndex].uv[i].uLeft = static_cast<float>(dst.x) / surfaceDim;
        fontInfo[fontIndex].uv[i].uRight = static_cast<float>(dst.x + characterWidth) / surfaceDim;
        fontInfo[fontIndex].uv[i].vTop = 1.0f - static_cast<float>(dst.y) / surfaceDim;
        fontInfo[fontIndex].uv[i].vBottom = 1.0f - static_cast<float>(dst.y + characterHeight) / surfaceDim;

        fontInfo[fontIndex].uv[i].width = characterWidth / (referenceWidth / 2.0f);

        dst.w = characterWidth;
        dst.h = characterHeight;

        SDL_BlitSurface(character, &src, targetSurface, &dst);

        dst.x += characterWidth;

        SDL_FreeSurface(character);
    }

    fontInfo[fontIndex].ascent = static_cast<float>(ascent) / static_cast<float>(surfaceDim);
    fontInfo[fontIndex].descent = static_cast<float>(descent) / static_cast<float>(surfaceDim);
    fontInfo[fontIndex].lineSkip = static_cast<float>(lineSkip) / static_cast<float>(surfaceDim);
    // Normalisiere die Höhe relativ zur Referenzauflösung (z.B. 768 Pixel Höhe)
    constexpr float referenceHeight = 768.0f;
    fontInfo[fontIndex].height = highestCharacter / (referenceHeight / 2.0f);

#if DEBUG_ATLAS
    // Speichere die Surface
    char surfaceFileName[256];
    std::filesystem::path debugDir = "./debug";
    if (!std::filesystem::exists(debugDir)) {
        std::filesystem::create_directories(debugDir);
    }

    snprintf(surfaceFileName, sizeof(surfaceFileName), "./debug/font_atlas_surface_%s_%d.bmp",
             TTFfontName.c_str(), fontSize);
    SDL_SaveBMP(targetSurface, surfaceFileName);
    SDL_Log("Font atlas surface gespeichert: %s", surfaceFileName);
#endif

    TextureUtils::SDL_FlipSurfaceVertical(targetSurface);

#if DEBUG_ATLAS
    // Speichere die inverted Surface vor der Texturerstellung
    char invertedSurfaceFileName[256];

    snprintf(invertedSurfaceFileName, sizeof(invertedSurfaceFileName), "./debug/font_atlas_inverted_surface_%s_%d.bmp",
             TTFfontName.c_str(), fontSize);
    SDL_SaveBMP(targetSurface, invertedSurfaceFileName);

    SDL_Log("Font atlas inverted surface gespeichert: %s", invertedSurfaceFileName);
#endif

    if (!TextureUtils::createGLTextureFromSurface(targetSurface, fontInfo[fontIndex].texture)) {
        SDL_FreeSurface(targetSurface);
        TTF_CloseFont(ttfFont);
        return false;
    }

#if DEBUG_ATLAS
    // save texture
    // Lese die Texturdaten direkt aus
    auto *pixels = new GLubyte[surfaceDim * surfaceDim * 4];
    glBindTexture(GL_TEXTURE_2D, fontInfo[fontIndex].tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Erstelle eine SDL_Surface aus den Pixeln
    SDL_Surface *textureSurface = SDL_CreateRGBSurfaceWithFormatFrom(
        pixels, surfaceDim, surfaceDim, 32,
        surfaceDim * 4, SDL_PIXELFORMAT_RGBA32);

    if (!textureSurface) {
        SDL_Log("Fehler beim Erstellen der Textur-Surface: %s", SDL_GetError());
        delete[] pixels;
    } else {
        // OpenGL speichert Bilder mit Y nach oben, aber BMP hat Y nach unten
        // Wir müssen also die Y-Achse umkehren für korrekte BMP-Darstellung
        TextureUtils::SDL_FlipSurfaceVertical(textureSurface);

        // Speichere die Textur
        char textureFileName[256];
        snprintf(textureFileName, sizeof(textureFileName), "./debug/font_atlas_texture_%s_%d.bmp",
                 TTFfontName.c_str(), fontSize);
        SDL_SaveBMP(textureSurface, textureFileName);
        SDL_Log("Font atlas texture gespeichert: %s", textureFileName);

        SDL_FreeSurface(textureSurface);
    }

    delete[] pixels;

#endif

    SDL_FreeSurface(targetSurface);
    TTF_CloseFont(ttfFont);
    return true;
}

void TextManager::write(const std::string &text, const Fonts font, const bool center, const GLfloat scale,
                        const GLfloat x,
                        const GLfloat y) const {
    const int fontIndex = static_cast<int>(font);
    unsigned char characterIndex;
    GLfloat sX, posX = 0;

    // Berechne die Breite für zentrierten Text
    if (center) {
        for (unsigned int i = 0; i < text.length(); i++) {
            characterIndex = static_cast<unsigned char>(text[i]);
            sX = fontInfo[fontIndex].uv[characterIndex].width * scale;
            posX += sX;
        }
        posX *= -0.5f; // Halbiere für echte Zentrierung
    }
    posX += x;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(posX, y - fontInfo[fontIndex].height * scale, 0.0f);
    glScalef(scale, scale, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontInfo[fontIndex].texture);

    GLfloat drawPosX = 0;

    unsigned int i = 0;
    while (i < text.length()) {
        constexpr unsigned char FALLBACK_CHAR = 127;
        const auto byte = static_cast<unsigned char>(text[i]);
        characterIndex = static_cast<unsigned char>(text[i]);
        if ((byte & 0x80) != 0) {
            // Multi-byte-chars
            const int byteCount = getUtf8ByteCount(byte);
            characterIndex = FALLBACK_CHAR;
            i += byteCount > 0 ? byteCount : 1; // Überspringe alle Bytes des Zeichens
        } else {
            // One-byte-Chars (ASCII)
            if (characterIndex < 32 || characterIndex > 127)
                characterIndex = FALLBACK_CHAR;
        }
        i++;

        sX = fontInfo[fontIndex].uv[characterIndex].width;

        // Baseline-Position verwenden statt fester Höhe
        const GLfloat baseline = fontInfo[fontIndex].ascent;

        // TODO: Descent berücksichtigen
        // const GLfloat descent = fontInfo[fontIndex].descent;
        const GLfloat charHeight = fontInfo[fontIndex].height;

        glBegin(GL_QUADS);

        //        +1
        //         ^
        //         |
        // -1 <----+----> +1
        //         |
        //        -1

        // (0, 1) ------- (1, 1)
        //   |              |
        //   |              |
        //   |              |
        // (0, 0) ------- (1, 0)

        // U = horizontal texture coordinate
        // V = vertical texture coordinate

        // Texturkoordinate (uLeft, vBottom) -> Vertex-Position unten links
        glTexCoord2f(fontInfo[fontIndex].uv[characterIndex].uLeft, fontInfo[fontIndex].uv[characterIndex].vBottom);
        glVertex3f(drawPosX, -baseline, 0.0f); // Unten links relativ zur Baseline

        // Texturkoordinate (uRight, vBottom) -> Vertex-Position unten rechts
        glTexCoord2f(fontInfo[fontIndex].uv[characterIndex].uRight, fontInfo[fontIndex].uv[characterIndex].vBottom);
        glVertex3f(drawPosX + sX, -baseline, 0.0f); // Unten rechts relativ zur Baseline

        // Texturkoordinate (uRight, vTop) -> Vertex-Position oben rechts
        glTexCoord2f(fontInfo[fontIndex].uv[characterIndex].uRight, fontInfo[fontIndex].uv[characterIndex].vTop);
        glVertex3f(drawPosX + sX, charHeight - baseline, 0.0f); // Oben rechts relativ zur Baseline

        // Texturkoordinate (uLeft, vTop) -> Vertex-Position oben links
        glTexCoord2f(fontInfo[fontIndex].uv[characterIndex].uLeft, fontInfo[fontIndex].uv[characterIndex].vTop);
        glVertex3f(drawPosX, charHeight - baseline, 0.0f); // Oben links relativ zur Baseline

        glEnd();

        // Kerning können wir nicht direkt implementieren, da ttfFont nicht verfügbar ist
        // Um Kerning zu unterstützen, müssten wir die Kerning-Werte in genFontTex vorberechnen
        // und in einer separaten Struktur speichern

        drawPosX += sX;
    }
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

GLfloat TextManager::getHeight(Fonts font) const {
    return fontInfo[static_cast<int>(font)].height;
}

void TextManager::clearTheme() {
    for (auto &i: fontInfo) {
        if (i.texture) {
            glDeleteTextures(1, &i.texture);
            i.texture = 0;
        }
    }
}

TextManager::~TextManager() {
    clearTheme();
    TTF_Quit();
}

bool TextManager::setTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    clearTheme();
    currentTheme = themeName;

    if (!loadAllFonts()) {
        SDL_Log("Error getting font theme: %s", currentTheme.c_str());
        return false;
    }
    return true;
}

Fonts TextManager::getFontTypeFromKey(const std::string &key) {
    if (key == "menusize") return Fonts::Menu;
    if (key == "announcegoodsize") return Fonts::AnnounceGood;
    if (key == "announcebadsize") return Fonts::AnnounceBad;
    if (key == "highscoresize") return Fonts::Highscore;
    if (key == "menuhighscoresize") return Fonts::MenuHighscore;
    if (key == "introhighscoresize") return Fonts::IntroHighscore;
    if (key == "introdescriptionsize") return Fonts::IntroDescription;
    return Fonts::Count;
}

bool TextManager::createFontTextures(const std::map<std::string, std::string> &fontFiles,
                                     const std::map<std::string, int> &fontSizes) {
    std::map<std::string, std::string> fontTypeMap = {
        {"menufile", "menusize"},
        {"announcegoodfile", "announcegoodsize"},
        {"announcebadfile", "announcebadsize"},
        {"highscorefile", "highscoresize"},
        {"menuhighscorefile", "menuhighscoresize"},
        {"introhighscorefile", "introhighscoresize"},
        {"introdescriptionfile", "introdescriptionsize"}
    };

    for (const auto &[fileKey, fileName]: fontFiles) {
        // Finde die zugehörige Größe
        std::string sizeKey = fontTypeMap[fileKey];
        auto sizeIt = fontSizes.find(sizeKey);
        if (sizeIt == fontSizes.end()) continue;

        // Font-Typ ermitteln und Textur generieren
        Fonts fontType = getFontTypeFromKey(sizeKey);
        if (fontType != Fonts::Count) {
            if (!genFontTex(fileName, sizeIt->second, fontType)) {
                SDL_Log("Error: generating texture %s", fileKey.c_str());
                return false;
            }
        }
    }
    return true;
}

bool TextManager::parseFontThemeDescriptionFile(const std::string &filePath,
                                                std::map<std::string, std::string> &fontFiles,
                                                std::map<std::string, int> &fontSizes) {
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        SDL_Log("Error: could not open font-description file: %s", filePath.c_str());
        return false;
    }

    std::string line;
    int lineNumber = 0;
    std::set<std::string> processedKeys; // Sammlung bereits verarbeiteter Schlüssel

    while (std::getline(configFile, line)) {
        lineNumber++;

        // Kommentare und leere Zeilen überspringen
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }

        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) continue;

        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);

        // Auf duplizierte Schlüssel prüfen
        if (processedKeys.find(key) != processedKeys.end()) {
            SDL_Log("Warning: ignoring duplicate key '%s' in line %d",
                    key.c_str(), lineNumber);
            continue;
        }

        processedKeys.insert(key);

        // Fonts speichern
        if (key.find("file") != std::string::npos) {
            fontFiles[key] = value;
        }
        // Größen speichern
        else if (key.find("size") != std::string::npos) {
            char *endptr = nullptr;
            long size = strtol(value.c_str(), &endptr, 10);
            if (*endptr != '\0') {
                SDL_Log("Not a number in fonts.txt: %s (line %d)",
                        value.c_str(), lineNumber);
                continue;
            }
            fontSizes[key] = static_cast<int>(size);
        }
    }
    configFile.close();
    return true;
}

bool TextManager::loadAllFonts() {
    const std::string fullPath = currentTheme + "/" + "font/fonts.txt";

    std::map<std::string, std::string> fontFiles;
    std::map<std::string, int> fontSizes;

    if (!parseFontThemeDescriptionFile(fullPath, fontFiles, fontSizes)) {
        return false;
    }

    return createFontTextures(fontFiles, fontSizes);
}

TextAnnouncement::TextAnnouncement(std::string msg, Fonts fontId, int ttl, TextManager *mgr)
    : message(std::move(msg)), font(fontId), lifetime(ttl), textManager(mgr) {
    age = 0;
    zoom = 0.0f;
    fade = 0.0f;
    fadingOut = false;
    active = true;
}

void TextAnnouncement::update(const float deltaTime) {
    // Zoom-Effekt mit deltaTime-Abhängigkeit
    zoom += 2.4f * deltaTime;

    // Fade-Effekt steuern
    if (fadingOut) {
        fade -= 1.0f * deltaTime;
    } else {
        fade += 1.0f * deltaTime;
    }

    // Fade-Werte begrenzen
    if (fade > 1.0f) fade = 1.0f;
    if (fade < 0.0f) fade = 0.0f;

    // Alter in Millisekunden
    age += deltaTime * 1000.0f;

    // Nach der Hälfte der Lebensdauer mit Fade-Out beginnen
    if (age > lifetime * 0.5f && !fadingOut) {
        fadingOut = true;
    }

    // Nach vollständiger Lebensdauer deaktivieren
    if (age > lifetime) {
        active = false;
    }
}

void TextAnnouncement::draw(const float deltaTime) const {
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    static float totalTime = 0.0f;
    totalTime += deltaTime; // Zeit akkumulieren

    // Erhöhter Skalierungsfaktor für bessere Sichtbarkeit
    const float baseScale = 0.8f;

    // Pulsieren als visuellen Effekt basierend auf deltaTime hinzufügen
    const float pulseEffect = sin(totalTime * 2.0f) * 0.15f;

    // Mehrere Ebenen mit verschiedenen Farben für den Schatteneffekt
    float s = zoom * baseScale * (0.85f + pulseEffect);
    glColor4f(1.0f, 0.0f, 0.0f, fade);

    const GLfloat fontHeight = textManager->getHeight(font);
    const float yPos = 0.0f + fontHeight;

    textManager->write(message, font, true, s, 0.0f, yPos);

    s = zoom * baseScale * (0.90f + pulseEffect);
    glColor4f(0.0f, 1.0f, 0.0f, fade);
    textManager->write(message, font, true, s, 0.0f, yPos);

    s = zoom * baseScale * (0.95f + pulseEffect);
    glColor4f(0.0f, 0.0f, 1.0f, fade);
    textManager->write(message, font, true, s, 0.0f, yPos);

    // Haupttext
    s = zoom * baseScale * (1.0f + pulseEffect);
    glColor4f(1.0f, 1.0f, 1.0f, fade);
    textManager->write(message, font, true, s, 0.0f, yPos);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void TextManager::addAnnouncement(const std::string &message, int lifetime, Fonts font) {
    // this-Zeiger übergeben statt getInstance() aufzurufen
    announcements.emplace_back(message, font, lifetime, this);
}

void TextManager::updateAnnouncements(const float deltaTime) {
    // Alle aktiven Announcements aktualisieren
    for (auto it = announcements.begin(); it != announcements.end();) {
        it->update(deltaTime);

        // Inaktive Announcements aus der Liste entfernen
        if (!it->isActive()) {
            it = announcements.erase(it);
        } else {
            ++it;
        }
    }
}

void TextManager::drawAnnouncements(const float deltaTime) {
    for (const auto &announcement: announcements) {
        announcement.draw(deltaTime);
    }
}

size_t TextManager::getAnnouncementCount() const {
    return announcements.size();
}
