#include "BackgroundManager.h"

#include <SDL2/SDL.h>
#include <string>
#include <epoxy/gl.h>
// #include "Texture.h"
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
    textureMgr = &texMgr;

    // 4 colors for the background, will be set randomly
    // Hier im Konstruktor, kann aber auch in updateBgIfNeeded
    for (int i = 0; i < 4; i++) {
        r[i] = 0.9f;
        g[i] = 0.9f;
        b[i] = 0.9f;
    }
    a = 0.95f;
}

bool BackgroundManager::updateBgIfNeeded(const Uint32 level, const std::filesystem::path &pathName) {
    const int bgNumber = level * 25.0f / 50.0f + 1;
    bool result;

    if (bgNumber > 25) {
        // Mh, final.jpg gibt es in themes/default/gfx/bg/ nicht
        const std::filesystem::path finalPath = pathName / "final.jpg";
        result = textureMgr->load(finalPath, tex);
    } else {
        const std::filesystem::path bgPath = pathName / (std::to_string(bgNumber) + ".jpg");
        result = textureMgr->load(bgPath, tex);
    }
    if (result) {
        if (backgroundDisplayList != 0) glDeleteLists(backgroundDisplayList, 1);
        backgroundDisplayList = glGenLists(1);
        glNewList(backgroundDisplayList, GL_COMPILE);
        drawQuad();
        glEndList();
    }
    return result;
}

void BackgroundManager::draw() const {
    if (backgroundDisplayList != 0) {
        glCallList(backgroundDisplayList);
    }
}

void BackgroundManager::drawQuad() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
    glBegin(GL_QUADS);
    // glColor4f(r[0], g[0], b[0], a);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    // glColor4f(r[1], g[1], b[1], a);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    // glColor4f(r[2], g[2], b[2], a);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    // glColor4f(r[3], g[3], b[3], a);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

BackgroundManager::~BackgroundManager() {
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }
}
