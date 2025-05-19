#include "PlayfieldBorder.h"
#include <epoxy/gl.h>
#include "colors.h"

PlayfieldBorder::PlayfieldBorder(Side side, const SpriteSheetAnimation& tex, float pillarWidth)
    : side(side), pillarWidth(pillarWidth)
{
    this->texture = tex;
    // Position und Größe werden ggf. vom aufrufenden Code gesetzt
}

void PlayfieldBorder::draw(float) {
    glLoadIdentity();
    glDisable(GL_BLEND);
    glColor4f(GL_WHITE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
    glBegin(GL_QUADS);

    if (side == Side::Left) {
        // linke Säule
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-1.0f + pillarWidth, 1.0f, 0.0f);
        glTexCoord2f(1.0f, -1.0f);
        glVertex3f(-1.0f + pillarWidth, -1.0f, 0.0f);
        glTexCoord2f(0.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
    } else {
        // rechte Säule
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f - pillarWidth, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, -1.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, -1.0f);
        glVertex3f(1.0f - pillarWidth, -1.0f, 0.0f);
    }
    glEnd();
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

