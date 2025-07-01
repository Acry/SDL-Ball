// SplashScreen.cpp
#include <epoxy/gl.h>
#include "SplashScreen.h"

SplashScreen::SplashScreen(TextureResource *texture)
    : textureProperties(texture) {
}

void SplashScreen::draw() const {
    if (!textureProperties) return;

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, textureProperties->id);

    glBegin(GL_QUADS);
    glTexCoord2f(textureProperties->uvCoordinates[0], textureProperties->uvCoordinates[1]);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(textureProperties->uvCoordinates[2], textureProperties->uvCoordinates[3]);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glTexCoord2f(textureProperties->uvCoordinates[4], textureProperties->uvCoordinates[5]);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(textureProperties->uvCoordinates[6], textureProperties->uvCoordinates[7]);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
