#include <epoxy/gl.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glu.h>
#include <fstream>
#include <filesystem>

#include "TextManager.h"

TextManager::TextManager() : fontInfo{} {
    TTF_Init();

    // Initialisiere alle Texturen mit 0
    for (int i = 0; i < FONT_NUM; i++) {
        fontInfo[i].tex = 0;
    }
}

void TextManager::genFontTex(const std::string &TTFfontName, const int fontSize, const int font) {
    std::string fullFontPath = fontThemePath + "/" + TTFfontName;

    TTF_Font *ttfFont = nullptr;
    SDL_Surface *c, *t;
    Uint32 rmask, gmask, bmask, amask;
    char tempChar[2] = {0, 0};
    int sX = 0, sY = 0; //Size of the rendered character
    SDL_Rect src = {0, 0, 0, 0}, dst = {0, 0, 0, 0};

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

    ttfFont = TTF_OpenFont(fullFontPath.c_str(), fontSize);
    if (!ttfFont) {
        SDL_Log("TTF_OpenFont: %s", TTF_GetError());
        // todo: return
    }
    t = SDL_CreateRGBSurface(0, 512, 512, 32, rmask, gmask, bmask, amask);

    dst.x = 1;
    dst.y = 1;

    fontInfo[font].height = 0.0;
    for (int i = 32; i < 128; i++) {
        constexpr SDL_Color white = {255, 255, 255, 255};
        tempChar[0] = static_cast<char>(i);

        //Render to surface
        c = TTF_RenderText_Blended(ttfFont, tempChar, white);
        SDL_SetSurfaceAlphaMod(c, 0xFF);
        TTF_SizeText(ttfFont, tempChar, &sX, &sY);

        src.x = 0;
        src.y = 0;
        src.w = sX;
        src.h = sY;

        if (dst.x + sX > 512) {
            dst.x = 1;
            dst.y += sY + 2;
            if (dst.y > 512 && font != FONT_HIGHSCORE) //FONT_HIGHSCORE is always rendered too big for texture
            {
                SDL_Log("Too many chars for tex (%d)", i);
            }
        }

        fontInfo[font].ch[i].Xa = (1.0f / (512.0f / static_cast<float>(dst.x)));
        fontInfo[font].ch[i].Xb = (1.0f / (512.0f / (static_cast<float>(dst.x) + sX)));
        fontInfo[font].ch[i].Ya = (1.0f / (512.0f / static_cast<float>(dst.y)));
        fontInfo[font].ch[i].Yb = (1.0f / (512.0f / (static_cast<float>(dst.y) + sY)));
        fontInfo[font].ch[i].width = sX / 800.0f;

        if (sY / 800.0 > fontInfo[font].height) {
            fontInfo[font].height = sY / 800.0f;
        }

        //blit
        dst.w = sX;
        dst.h = sY;
        SDL_BlitSurface(c, &src, t, &dst);

        dst.x += sX + 2; // Waste some space 1 px padding around each char

        SDL_FreeSurface(c); //Free character-surface
    }

    glGenTextures(1, &fontInfo[font].tex); //Generate a gltexture for this font

    glBindTexture(GL_TEXTURE_2D, fontInfo[font].tex);
    // gluBuild2DMipmapLevels
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, t->w, t->h, GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);

    TTF_CloseFont(ttfFont); //Free the font
    SDL_FreeSurface(t); //Free text-surface
}

void TextManager::write(const std::string &text, const int font, const bool center, const GLfloat scale,
                        const GLfloat x,
                        const GLfloat y) const {
    unsigned char c;
    GLfloat sX, posX = 0;
    // Disable depth test for UI elements if you want them always on top
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // find out half of the string width to center
    if (center) {
        for (unsigned int i = 0; i < text.length(); i++) {
            c = static_cast<unsigned char>(text[i]);
            sX = fontInfo[font].ch[c].width * scale;
            posX += sX;
        }
        posX *= -1;
    }
    posX += x;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(posX, y, 0.0f);
    glScalef(scale, scale, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontInfo[font].tex);

    GLfloat drawPosX = 0;
    for (unsigned char i = 0; i < text.length(); i++) {
        c = static_cast<unsigned char>(text[i]);
        if (c < 32 || c >= 128) c = '?';
        sX = fontInfo[font].ch[c].width;
        const GLfloat sY = fontInfo[font].height;
        drawPosX += sX;
        constexpr float zFront = -0.15f;
        glBegin(GL_QUADS);
        glTexCoord2f(fontInfo[font].ch[c].Xa, fontInfo[font].ch[c].Ya);
        glVertex3f(-sX + drawPosX, sY, zFront);
        glTexCoord2f(fontInfo[font].ch[c].Xb, fontInfo[font].ch[c].Ya);
        glVertex3f(sX + drawPosX, sY, zFront);
        glTexCoord2f(fontInfo[font].ch[c].Xb, fontInfo[font].ch[c].Yb);
        glVertex3f(sX + drawPosX, -sY, zFront);
        glTexCoord2f(fontInfo[font].ch[c].Xa, fontInfo[font].ch[c].Yb);
        glVertex3f(-sX + drawPosX, -sY, zFront);
        glEnd();
        drawPosX += sX;
    }
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

GLfloat TextManager::getHeight(const int font) const {
    return fontInfo[font].height;
}

void TextManager::clearFontTheme() {
    // Bestehende Texturen freigeben, wenn vorhanden
    for (int i = 0; i < FONT_NUM; i++) {
        if (fontInfo[i].tex) {
            glDeleteTextures(1, &fontInfo[i].tex);
            fontInfo[i].tex = 0;
        }
    }
}

TextManager::~TextManager() {
    clearFontTheme();
    TTF_Quit();
}

bool TextManager::setFontTheme(const std::string &fontFilePath) {
    // Bestehende Ressourcen freigeben
    clearFontTheme();

    // Font-Basispfad ermitteln (alles bis zum letzten /)
    std::filesystem::path path(fontFilePath);
    fontThemePath = path.parent_path().string();

    // Parse font-description file
    std::ifstream f(fontFilePath);
    if (!f.is_open()) {
        SDL_Log("Error: could not open font-description file: %s", fontFilePath.c_str());
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
                int font = -1;
                if (set == "menusize") font = FONT_MENU;
                else if (set == "announcegoodsize") font = FONT_ANNOUNCE_GOOD;
                else if (set == "announcebadsize") font = FONT_ANNOUNCE_BAD;
                else if (set == "highscoresize") font = FONT_HIGHSCORE;
                else if (set == "menuhighscoresize") font = FONT_MENUHIGHSCORE;
                else if (set == "introhighscoresize") font = FONT_INTROHIGHSCORE;
                else if (set == "introdescriptionsize") font = FONT_INTRODESCRIPTION;
                if (font != -1)
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

TextAnnouncement::TextAnnouncement(const std::string& msg, int fontId, int ttl, TextManager* mgr)
    : message(msg), font(fontId), lifetime(ttl), textManager(mgr) {
    age = 0;
    zoom = 0.0f;
    fade = 0.0f;
    fadingOut = false;
    active = true;
}

void TextAnnouncement::update(float deltaTime) {
    // Zooming-Geschwindigkeit angepasst an orthogonale Projektion
    zoom += 1.0f * deltaTime;

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

void TextAnnouncement::draw() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Angepasster Skalierungsfaktor für orthogonale Projektion
    float baseScale = 0.27f;

    // Mehrere Ebenen mit verschiedenen Farben für den Schatteneffekt
    float s = zoom * baseScale * 0.85f;
    glColor4f(1.0f, 0.0f, 0.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    s = zoom * baseScale * 0.90f;
    glColor4f(0.0f, 1.0f, 0.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    s = zoom * baseScale * 0.95f;
    glColor4f(0.0f, 0.0f, 1.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    // Haupttext
    s = zoom * baseScale;
    glColor4f(1.0f, 1.0f, 1.0f, fade);
    textManager->write(message, font, true, s, 0.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glDisable(GL_BLEND);
}

void TextManager::addAnnouncement(const std::string& message, int lifetime, int font) {
    // this-Zeiger übergeben statt getInstance() aufzurufen
    announcements.emplace_back(message, font, lifetime, this);
}

void TextManager::updateAnnouncements(float deltaTime) {
    // Alle aktiven Announcements aktualisieren
    for (auto it = announcements.begin(); it != announcements.end(); ) {
        it->update(deltaTime);

        // Inaktive Announcements aus der Liste entfernen
        if (!it->isActive()) {
            it = announcements.erase(it);
        } else {
            ++it;
        }
    }
}

void TextManager::drawAnnouncements() {
    // Alle aktiven Announcements zeichnen
    for (auto& announcement : announcements) {
        announcement.draw();
    }
}