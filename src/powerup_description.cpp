// powerup_description.cpp
#include "powerup_description.h"
#include "MovingObject.h"
#include <GL/gl.h>

powerupDescriptionClass::powerupDescriptionClass() : text(Text::getInstance()) {
    width = 0.035;
    height = 0.035;
}

void powerupDescriptionClass::draw()  {
    // draw powerup icon, name and description
    tex->play();
    glLoadIdentity();
    glEnable( GL_TEXTURE_2D );
    glBindTexture(GL_TEXTURE_2D, tex->prop.texture);
    glColor4f(tex->prop.glTexColorInfo[0], tex->prop.glTexColorInfo[1], tex->prop.glTexColorInfo[2],
              tex->prop.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(tex->pos[0], tex->pos[1]);
    glVertex3f(-width + posx, height + posy, 0.00); // top left
    glTexCoord2f(tex->pos[2], tex->pos[3]);
    glVertex3f(width + posx, height + posy, 0.00); // top right
    glTexCoord2f(tex->pos[4], tex->pos[5]);
    glVertex3f(width + posx, -height + posy, 0.00); // bottom right
    glTexCoord2f(tex->pos[6], tex->pos[7]);
    glVertex3f(-width + posx, -height + posy, 0.00); // bottom left
    glEnd();
    glDisable( GL_TEXTURE_2D );
    constexpr float spacing = 0.05f;
    // Write the name and description
    constexpr float scale = 1.0f; // Scale for the text
    constexpr float leading = -0.018f; // Leading for the text
    const float textX = posx + spacing;
    text.write(name,
                  FONT_INTRODESCRIPTION,
                  false,
                  scale, textX,
                  posy - leading);
    text.write(description,
                  FONT_INTRODESCRIPTION,
                  false,
                  scale,
                  textX,
                  posy - height - leading);
}