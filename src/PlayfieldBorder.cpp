#include "PlayfieldBorder.h"
#include <epoxy/gl.h>
#include "colors.h"
#include "config.h"

PlayfieldBorder::PlayfieldBorder(Side side, const SpriteSheetAnimation &tex)
    : side(side) {
    this->texture = tex;
    init();
}

void PlayfieldBorder::init() {
    dl = glGenLists(1);
    glNewList(dl, GL_COMPILE);
    glLoadIdentity();
    //glColor4f(GL_WHITE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
    glBegin(GL_QUADS);

    if (side == Side::Left) {
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-1.0f + PILLAR_WIDTH, 1.0f, 0.0f);
        glTexCoord2f(1.0f, -1.0f);
        glVertex3f(-1.0f + PILLAR_WIDTH, -1.0f, 0.0f);
        glTexCoord2f(0.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
    } else {
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f - PILLAR_WIDTH, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, -1.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, -1.0f);
        glVertex3f(1.0f - PILLAR_WIDTH, -1.0f, 0.0f);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void PlayfieldBorder::draw(float) {
    if (dl)
        glCallList(dl);
}
