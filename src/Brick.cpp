#include <epoxy/gl.h>
#include "Brick.h"

void Brick::draw() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, textureProperties.id);
    glColor4fv(textureProperties.textureColor);
    glBegin(GL_QUADS);

    // Bottom-left corner
    glTexCoord2f(textureProperties.uvCoordinates[0], textureProperties.uvCoordinates[1]);
    glVertex3f(pos_x, pos_y, 0.0f);

    // Bottom-right corner
    glTexCoord2f(textureProperties.uvCoordinates[2], textureProperties.uvCoordinates[3]);
    glVertex3f(pos_x + width, pos_y, 0.0f);

    // Top-right corner
    glTexCoord2f(textureProperties.uvCoordinates[4], textureProperties.uvCoordinates[5]);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);

    // Top-left corner
    glTexCoord2f(textureProperties.uvCoordinates[6], textureProperties.uvCoordinates[7]);
    glVertex3f(pos_x, pos_y + height, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
