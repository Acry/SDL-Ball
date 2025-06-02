#include <SDL2/SDL.h>
#include <string>
#include <epoxy/gl.h>
#include <variant>

#include "BackgroundManager.h"
#include "TextureManager.h"

// zu, setting.showBg = false;
// Gibt der TextureManager ein false zurück. lief irgendetwas schief und runtime-setting.showBg = false
// Beim nächsten Update wird der BackgroundManager nicht mehr gezeichnet. Aber nach dem Level wird BGManager
// updateBgIfNeeded erneut versucht. Vielleicht gab es nur ein Problem mit einem Hintergrund.
// Wir geben in den BG-Manager ein Setting, ob das Farben des Hintergrunds (original-Fassung) gewünscht ist.
//
// Das war der alte Code
// Den Wert `taint_bg_color` fügen wir der Config zu
// for(int i = 0; i  < 4; i++)
//   {
//       r[i] = rndflt(1,0);
//       g[i] = rndflt(1,0);
//       b[i] = rndflt(1,0);
//   }
//
// a=
// 1;

// Den ThemenPfad geben wir ebenfalls von außen rein.
// Die Anzahl der Hintergründe sollte nicht fix sein.
// Auflösung und Format spielen keine Rolle, lediglich das AR von 4:3 ist wichtig.
// 4:3 kann auch einfach der Mitte genommen werden.
// Die Anzahl der Hintergründe wird gelesen. Sollte es ein Final.jpg wird das berücksichtigt.
// Also sollten wir den Texture Manager als Abhängigkeit haben.
BackgroundManager::BackgroundManager(const TextureManager &texMgr) : backgroundDisplayList(0) {
    textureManager = &texMgr;
    // 4 colors for the background, will be set randomly
    // Hier im Konstruktor, kann aber auch in updateBgIfNeeded
    for (int i = 0; i < 4; i++) {
        r[i] = 0.9f;
        g[i] = 0.9f;
        b[i] = 0.9f;
    }
    a = 0.95f;
}

bool BackgroundManager::updateBgIfNeeded(Uint32 level) {
    const size_t bgCount = textureManager->getBackgroundCount();
    if (bgCount == 0) {
        SDL_Log("Error: No backgrounds available in the TextureManager.");
        return false;
    };
    level += 1;
    if (level >= maxLevel) {
        const SpriteSheetAnimation *background = textureManager->getBackground(bgCount - 1);
        if (!background) {
            SDL_Log("Error: Could not get final background texture");
            return false;
        }
        texture = *background;
    } else {
        // Gleichmäßige Verteilung der nicht-finalen Hintergründe
        const size_t regularBgCount = bgCount - 1; // Anzahl der regulären Hintergründe
        const size_t levelsPerBg = maxLevel / regularBgCount;
        const size_t bgIndex = (level - 1) / levelsPerBg;

        // Sicherstellen, dass wir nicht über das Array hinausgehen
        const size_t safeBgIndex = std::min(bgIndex, regularBgCount - 1);

        const SpriteSheetAnimation *background = textureManager->getBackground(safeBgIndex);
        if (!background) {
            SDL_Log("Error: Could not get background texture for index %zu", bgIndex);
            return false;
        }
        texture = *background;
    }
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }
    backgroundDisplayList = glGenLists(1);
    glNewList(backgroundDisplayList, GL_COMPILE);
    drawQuad();
    glEndList();

    return true;
}

void BackgroundManager::draw() const {
    if (backgroundDisplayList != 0) {
        glCallList(backgroundDisplayList);
    }
}

void BackgroundManager::drawQuad() {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);

    glBegin(GL_QUADS);

    // Bottom-left corner.
    // glColor4f(r[0], g[0], b[0], a);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    // Bottom-right corner.
    // glColor4f(r[1], g[1], b[1], a);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    // Top-right corner.
    // glColor4f(r[2], g[2], b[2], a);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

    // Top-left corner.
    // glColor4f(r[3], g[3], b[3], a);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

BackgroundManager::~BackgroundManager() {
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }
}

void BackgroundManager::registerEvents(EventManager *em) {
    if (!em) return;

    eventManager = em;

    // Level-Change Event
    eventManager->addListener(GameEvent::LevelChanged, [this](const LevelEventData &data) {
        this->maxLevel = data.maxLevel;
        this->updateBgIfNeeded(data.currentlevel);
    }, this);

    // Theme-Change Event
    eventManager->addListener(GameEvent::LevelThemeChanged, [this](const LevelEventData &data) {
        this->maxLevel = data.maxLevel;
        this->updateBgIfNeeded(data.currentlevel);
    }, this);
}
