#include <epoxy/gl.h>
#include "Brick.h"

void Brick::drawBase() const {
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->textureProperties.texture);
    glColor4f(texture->textureProperties.glTexColorInfo[0],
              texture->textureProperties.glTexColorInfo[1],
              texture->textureProperties.glTexColorInfo[2],
              texture->textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);

    // Bottom-left corner.
    glTexCoord2f(texture->texturePosition[0], texture->texturePosition[1]);
    glVertex3f(pos_x, pos_y, 0.0f);

    // Bottom-right corner.
    glTexCoord2f(texture->texturePosition[2], texture->texturePosition[3]);
    glVertex3f(pos_x + width, pos_y, 0.0f);

    // Top-right corner.
    glTexCoord2f(texture->texturePosition[4], texture->texturePosition[5]);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);

    // Top-left corner.
    glTexCoord2f(texture->texturePosition[6], texture->texturePosition[7]);
    glVertex3f(pos_x, pos_y + height, 0.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Brick::draw(const float deltaTime) {
    texture->play(deltaTime);
    drawBase();
}
