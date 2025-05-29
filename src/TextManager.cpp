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

TextManager::TextManager() : fontInfo{} {
    TTF_Init();

    // Initialisiere alle Texturen mit 0
    for (int i = 0; i < static_cast<int>(Fonts::Count); i++) {
        fontInfo[i].tex = 0;
    }
}

bool TextManager::genFontTex(const std::string &TTFfontName, const int fontSize, const Fonts font) {
    std::string fullPath = currentTheme + "/" + "font/" + TTFfontName;

    TTF_Font *ttfFont = nullptr;
    SDL_Surface *c, *invertedC, *t;
    Uint32 rmask, gmask, bmask, amask;
    char tempChar[2] = {0, 0};
    int sX = 0, sY = 0; //Size of the rendered character
    SDL_Rect src = {0, 0, 0, 0}, dst = {0, 0, 0, 0};
    const int fontIndex = static_cast<int>(font);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif


    ttfFont = TTF_OpenFont(fullPath.c_str(), fontSize);
    if (!ttfFont) {
        SDL_Log("TTF_OpenFont: %s", TTF_GetError());
        return false;
    }
    TTF_SetFontHinting(ttfFont, TTF_HINTING_LIGHT_SUBPIXEL);
    int surfaceDim = 1024; // Size of the surface to render the font to
    t = SDL_CreateRGBSurface(0, surfaceDim, surfaceDim, 32, rmask, gmask, bmask, amask);

    dst.x = 1;
    dst.y = 1;

    // Anfangshöhe auf Null setzen, wir ermitteln die maximale Höhe während der Schleife
    fontInfo[fontIndex].height = 0.0f;
    float maxHeight = 0.0f;

    for (int i = 32; i < 128; i++) {
        constexpr SDL_Color white = {255, 255, 255, 255};
        tempChar[0] = static_cast<char>(i);

        //Render to surface
        c = TTF_RenderText_Blended(ttfFont, tempChar, white);
        SDL_SetSurfaceAlphaMod(c, 0xFF);
        TTF_SizeText(ttfFont, tempChar, &sX, &sY);

        // Y-invertierte Surface erstellen
        invertedC = TextureUtils::invertSurfaceY(c);
        if (!invertedC) {
            SDL_FreeSurface(c);
            continue;
        }

        // Pixel-Höhe für die maximale Höhenberechnung speichern
        if (sY > maxHeight) {
            maxHeight = static_cast<float>(sY);
        }

        src.x = 0;
        src.y = 0;
        src.w = sX;
        src.h = sY;

        if (dst.x + sX > surfaceDim) {
            dst.x = 1;
            dst.y += sY + 2;
            if (dst.y > surfaceDim) {
                SDL_Log("Too many chars for tex (%d)", i);
            }
        }

        fontInfo[fontIndex].ch[i].Xa = (1.0f / (surfaceDim / static_cast<float>(dst.x)));
        fontInfo[fontIndex].ch[i].Xb = (1.0f / (surfaceDim / (static_cast<float>(dst.x) + sX)));
        fontInfo[fontIndex].ch[i].Ya = (1.0f / (surfaceDim / static_cast<float>(dst.y)));
        fontInfo[fontIndex].ch[i].Yb = (1.0f / (surfaceDim / (static_cast<float>(dst.y) + sY)));
        fontInfo[fontIndex].ch[i].width = sX / static_cast<float>(surfaceDim);

        //blit
        dst.w = sX;
        dst.h = sY;
        SDL_BlitSurface(invertedC, &src, t, &dst);

        dst.x += sX + 2; // Waste some space 1 px padding around each char

        SDL_FreeSurface(invertedC);
        SDL_FreeSurface(c);
    }

    // Setze die finale Höhe nach derselben Normalisierungsmethode wie bei der Breite
    fontInfo[fontIndex].height = maxHeight / static_cast<float>(surfaceDim);

    if (!TextureUtils::createGLTextureFromSurface(t, fontInfo[fontIndex].tex)) {
        SDL_FreeSurface(t);
        TTF_CloseFont(ttfFont);
        return false;
    }

    SDL_FreeSurface(t);
    TTF_CloseFont(ttfFont);
    return true;
}
void TextManager::write(const std::string &text, const Fonts font, const bool center, const GLfloat scale,
                        const GLfloat x,
                        const GLfloat y) const {
    int fontIndex = static_cast<int>(font);
    unsigned char c;
    GLfloat sX, posX = 0;
    // Berechne die Breite für zentrierten Text
    if (center) {
        for (unsigned int i = 0; i < text.length(); i++) {
            c = static_cast<unsigned char>(text[i]);
            sX = fontInfo[fontIndex].ch[c].width * scale;
            posX += sX;
        }
        posX *= -1;
    }
    posX += x;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(posX, y, 0.0f);
    glScalef(scale, scale, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontInfo[fontIndex].tex);

    GLfloat drawPosX = 0;
    for (unsigned char i = 0; i < text.length(); i++) {
        c = static_cast<unsigned char>(text[i]);
        if (c < 32 || c >= 128) c = '?';
        sX = fontInfo[fontIndex].ch[c].width;
        const GLfloat sY = fontInfo[fontIndex].height;
        drawPosX += sX;

        glBegin(GL_QUADS);
        // Änderung hier: Y-Koordinaten umgekehrt, damit Text nach unten gerendert wird
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xa, fontInfo[fontIndex].ch[c].Ya);
        glVertex3f(-sX + drawPosX, -sY, 0.0f); // Früher +sY, jetzt -sY
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xb, fontInfo[fontIndex].ch[c].Ya);
        glVertex3f(sX + drawPosX, -sY, 0.0f); // Früher +sY, jetzt -sY
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xb, fontInfo[fontIndex].ch[c].Yb);
        glVertex3f(sX + drawPosX, sY, 0.0f); // Früher -sY, jetzt +sY
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xa, fontInfo[fontIndex].ch[c].Yb);
        glVertex3f(-sX + drawPosX, sY, 0.0f); // Früher -sY, jetzt +sY
        glEnd();
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
    // Bestehende Texturen freigeben, wenn vorhanden
    for (auto &i: fontInfo) {
        if (i.tex) {
            glDeleteTextures(1, &i.tex);
            i.tex = 0;
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
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    static float totalTime = 0.0f;
    totalTime += deltaTime; // Zeit akkumulieren

    // Erhöhter Skalierungsfaktor für bessere Sichtbarkeit
    float baseScale = 0.8f;

    // Pulsieren als visuellen Effekt basierend auf deltaTime hinzufügen
    float pulseEffect = sin(totalTime * 2.0f) * 0.15f;

    // Mehrere Ebenen mit verschiedenen Farben für den Schatteneffekt
    float s = zoom * baseScale * (0.85f + pulseEffect);
    glColor4f(1.0f, 0.0f, 0.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    s = zoom * baseScale * (0.90f + pulseEffect);
    glColor4f(0.0f, 1.0f, 0.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    s = zoom * baseScale * (0.95f + pulseEffect);
    glColor4f(0.0f, 0.0f, 1.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    // Haupttext
    s = zoom * baseScale * (1.0f + pulseEffect);
    glColor4f(1.0f, 1.0f, 1.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

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
