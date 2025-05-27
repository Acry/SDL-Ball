// powerup_description.cpp
#include "powerup_description.h"
#include "MovingObject.h"
#include <GL/gl.h>

powerupDescriptionClass::powerupDescriptionClass() : text(TextManager::getInstance()) {
    width = 0.035;
    height = 0.035;
}

void powerupDescriptionClass::draw() {
    // draw powerup icon, name and description
    tex->play();
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->textureProperties.texture);
    glColor4f(tex->textureProperties.glTexColorInfo[0], tex->textureProperties.glTexColorInfo[1],
              tex->textureProperties.glTexColorInfo[2],
              tex->textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(tex->texturePosition[0], tex->texturePosition[1]);
    glVertex3f(-width + pos_x, height + pos_y, 0.00); // top left
    glTexCoord2f(tex->texturePosition[2], tex->texturePosition[3]);
    glVertex3f(width + pos_x, height + pos_y, 0.00); // top right
    glTexCoord2f(tex->texturePosition[4], tex->texturePosition[5]);
    glVertex3f(width + pos_x, -height + pos_y, 0.00); // bottom right
    glTexCoord2f(tex->texturePosition[6], tex->texturePosition[7]);
    glVertex3f(-width + pos_x, -height + pos_y, 0.00); // bottom left
    glEnd();
    glDisable(GL_TEXTURE_2D);
    constexpr float spacing = 0.05f;
    // Write the name and description
    constexpr float scale = 1.0f; // Scale for the text
    constexpr float leading = -0.018f; // Leading for the text
    const float textX = pos_x + spacing;
    text.write(name,
               FONT_INTRODESCRIPTION,
               false,
               scale, textX,
               pos_y - leading);
    text.write(description,
               FONT_INTRODESCRIPTION,
               false,
               scale,
               textX,
               pos_y - height - leading);
}
