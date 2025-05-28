// TextManager.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <filesystem>
#include <utility>

#include "TextManager.h"

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
    SDL_Surface *c, *t;
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
        SDL_BlitSurface(c, &src, t, &dst);

        dst.x += sX + 2; // Waste some space 1 px padding around each char

        SDL_FreeSurface(c); //Free character-surface
    }

    // Setze die finale Höhe nach derselben Normalisierungsmethode wie bei der Breite
    fontInfo[fontIndex].height = maxHeight / static_cast<float>(surfaceDim);

    glGenTextures(1, &fontInfo[fontIndex].tex); //Generate a gltexture for this font

    glBindTexture(GL_TEXTURE_2D, fontInfo[fontIndex].tex);
    glBindTexture(GL_TEXTURE_2D, fontInfo[fontIndex].tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    TTF_CloseFont(ttfFont); //Free the font
    SDL_FreeSurface(t); //Free text-surface
    return true;
}

void TextManager::write(const std::string &text, const Fonts font, const bool center, const GLfloat scale,
                        const GLfloat x,
                        const GLfloat y) const {
    int fontIndex = static_cast<int>(font);
    unsigned char c;
    GLfloat sX, posX = 0;
    // find out half of the string width to center
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
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xa, fontInfo[fontIndex].ch[c].Ya);
        glVertex3f(-sX + drawPosX, sY, 0.0f);
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xb, fontInfo[fontIndex].ch[c].Ya);
        glVertex3f(sX + drawPosX, sY, 0.0f);
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xb, fontInfo[fontIndex].ch[c].Yb);
        glVertex3f(sX + drawPosX, -sY, 0.0f);
        glTexCoord2f(fontInfo[fontIndex].ch[c].Xa, fontInfo[fontIndex].ch[c].Yb);
        glVertex3f(-sX + drawPosX, -sY, 0.0f);
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
    for (int i = 0; i < static_cast<int>(Fonts::Count); i++) {
        if (fontInfo[i].tex) {
            glDeleteTextures(1, &fontInfo[i].tex);
            fontInfo[i].tex = 0;
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
        SDL_Log("Fehler beim Laden der Texturen für Theme: %s", currentTheme.c_str());
        return false;
    }
    return true;
}

bool TextManager::loadAllFonts() {
    std::string fullPath = currentTheme + "/" + "font/fonts.txt";

    // Parse font-description file
    std::ifstream f(fullPath);
    if (!f.is_open()) {
        SDL_Log("Error: could not open font-description file: %s", fullPath.c_str());
        return false;
    }

    std::string val;
    std::string tempName;
    std::string set;
    std::string line;

    while (std::getline(f, line)) {
        if (line.find('=') != std::string::npos) {
            set = line.substr(0, line.find('='));
            val = line.substr(line.find('=') + 1);
            if (set == "menufile") {
                tempName = val;
            } else if (
                set == "menusize" ||
                set == "announcegoodsize" ||
                set == "announcebadsize" ||
                set == "highscoresize" ||
                set == "menuhighscoresize" ||
                set == "introhighscoresize" ||
                set == "introdescriptionsize"
            ) {
                char *endptr = nullptr;
                long size = strtol(val.c_str(), &endptr, 10);
                if (*endptr != '\0') {
                    SDL_Log("Ungültige Zahl in fonts.txt: %s", val.c_str());
                    continue;
                }
                auto font = Fonts::Count;
                if (set == "menusize") font = Fonts::Menu;
                else if (set == "announcegoodsize") font = Fonts::AnnounceGood;
                else if (set == "announcebadsize") font = Fonts::AnnounceBad;
                else if (set == "highscoresize") font = Fonts::Highscore;
                else if (set == "menuhighscoresize") font = Fonts::MenuHighscore;
                else if (set == "introhighscoresize") font = Fonts::IntroHighscore;
                else if (set == "introdescriptionsize") font = Fonts::IntroDescription;
                if (font != Fonts::Count)
                    genFontTex(tempName, static_cast<int>(size), font);
            } else if (
                set == "announcegoodfile" ||
                set == "announcebadfile" ||
                set == "highscorefile" ||
                set == "menuhighscorefile" ||
                set == "introhighscorefile" ||
                set == "introdescriptionfile"
            ) {
                tempName = val;
            }
        }
    }
    f.close();
    return true;
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
