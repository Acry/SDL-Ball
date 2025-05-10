#include "Background.h"

#include <SDL2/SDL.h>
#include <string>
#include <epoxy/gl.h>
#include "Texture.h"
#include "TextureManager.h"
#include "ThemeManager.h"
#include "game_state.h"
#include "SettingsManager.h"

extern ThemeManager themeManager;
extern settings setting;

Background::Background() : displayList(0), currentBgNumber(-1) {
    for (int i = 0; i < 4; i++) {
        r[i] = 0.9f;
        g[i] = 0.9f;
        b[i] = 0.9f;
    }
    a = 0.95f;
}

Background::~Background() {
    if (displayList != 0) {
        glDeleteLists(displayList, 1);
    }
}

bool Background::updateBgIfNeeded(TextureManager &texMgr) {
    const int bgNumber = static_cast<int>(player.level * 25.0f / 50.0f) + 1;
    bool result;

    if (bgNumber > 25) {
        result = texMgr.load(themeManager.getThemeFilePath("gfx/bg/final.jpg", setting.gfxTheme), tex);
    } else {
        const std::string filename = "gfx/bg/" + std::to_string(bgNumber) + ".jpg";
        result = texMgr.load(themeManager.getThemeFilePath(filename.c_str(), setting.gfxTheme), tex);
    }

    if (result) {
        if (displayList != 0) glDeleteLists(displayList, 1);
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE);
        drawQuad();
        glEndList();
    }
    return result;
}

void Background::init(TextureManager &texMgr) {
    if (!updateBgIfNeeded(texMgr)) {
        setting.showBg = false;
        SDL_Log("Background loading failed.");
    }
}

void Background::draw() const {
    if (displayList != 0) {
        glCallList(displayList);
    }
}

void Background::drawQuad() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
    glBegin(GL_QUADS);
    glColor4f(r[0], g[0], b[0], a);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glColor4f(r[1], g[1], b[1], a);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glColor4f(r[2], g[2], b[2], a);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glColor4f(r[3], g[3], b[3], a);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
