#include "Paddle.h"
#include <epoxy/gl.h>

Paddle::Paddle() {
    init();
}

void Paddle::init() {
    growing = false;
    growspeed = 0.05f;
    aspect = 0.2f;
    pos_y = -0.93f;
    pos_x = 0.0f;
    width = 0.059f;
    height = 0.018f;
    dead = false;
    // Initialisierung der Layer-Flags
    hasGlueLayer = false;
    hasGunLayer = false;
}

void Paddle::grow(const GLfloat width) {
    destwidth = width;
    growing = true;
}

void Paddle::updateGrowth(const float deltaTime) {
    if (growing) {
        const GLfloat growthRate = growspeed * deltaTime;

        // Bestimme ob wir wachsen oder schrumpfen müssen
        if (width < destwidth) {
            // Wachsen
            width += growthRate;
            if (width >= destwidth) {
                width = destwidth;
                growing = false;
            }
        } else if (width > destwidth) {
            // Schrumpfen
            width -= growthRate;
            if (width <= destwidth) {
                width = destwidth;
                growing = false;
            }
        } else {
            growing = false;
        }

        // Höhe entsprechend dem Seitenverhältnis anpassen
        height = aspect * width;
    }
}

void Paddle::drawBase() {
    texture.play();
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
    glColor4f(texture.textureProperties.glTexColorInfo[0],
              texture.textureProperties.glTexColorInfo[1],
              texture.textureProperties.glTexColorInfo[2],
              texture.textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(texture.texturePosition[0], texture.texturePosition[1]);
    glVertex3f(-width, height, 0.0f);
    glTexCoord2f(texture.texturePosition[2], texture.texturePosition[3]);
    glVertex3f(width, height, 0.0f);
    glTexCoord2f(texture.texturePosition[4], texture.texturePosition[5]);
    glVertex3f(width, -height, 0.0f);
    glTexCoord2f(texture.texturePosition[6], texture.texturePosition[7]);
    glVertex3f(-width, -height, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Paddle::drawGlueLayer() const {
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, layerTex[0].textureProperties.texture);
    glColor4f(layerTex[0].textureProperties.glTexColorInfo[0],
              layerTex[0].textureProperties.glTexColorInfo[1],
              layerTex[0].textureProperties.glTexColorInfo[2],
              layerTex[0].textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-width, height, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, height, 0.0f);
    glTexCoord2f(1.0f, 0.99f);
    glVertex3f(width, -height, 0.0f);
    glTexCoord2f(0.0f, 0.99f);
    glVertex3f(-width, -height, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Paddle::drawGunLayer() const {
    layerTex[1].play();
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, layerTex[1].textureProperties.texture);
    glColor4f(layerTex[1].textureProperties.glTexColorInfo[0],
              layerTex[1].textureProperties.glTexColorInfo[1],
              layerTex[1].textureProperties.glTexColorInfo[2],
              layerTex[1].textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(layerTex[1].texturePosition[0], layerTex[1].texturePosition[1]);
    glVertex3f(-width, height * 4, 0.0f);
    glTexCoord2f(layerTex[1].texturePosition[2], layerTex[1].texturePosition[3]);
    glVertex3f(width, height * 4, 0.0f);
    glTexCoord2f(layerTex[1].texturePosition[4], layerTex[1].texturePosition[5] - 0.01f);
    glVertex3f(width, height, 0.0f);
    glTexCoord2f(layerTex[1].texturePosition[6], layerTex[1].texturePosition[7] - 0.01f);
    glVertex3f(-width, height, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Paddle::draw() {
    if (dead) return;

    drawBase();

    if (hasGlueLayer)
        drawGlueLayer();

    if (hasGunLayer)
        drawGunLayer();
}

void Paddle::setGlueLayer(const bool enabled) {
    hasGlueLayer = enabled;
}

void Paddle::setGunLayer(const bool enabled) {
    hasGunLayer = enabled;
}

void Paddle::update(float deltaTime) {
    // Hier Paddle-Logik implementieren
    updateGrowth(deltaTime);
}

void Paddle::moveTo(float targetX, float deltaTime) {
    // Bewegungsgeschwindigkeit basierend auf deltaTime
    float speed = 2.0f; // Anpassbare Geschwindigkeit
    float movement = speed * deltaTime;

    // Bewege in Richtung Zielposition
    if (pos_x < targetX) {
        pos_x = std::min(pos_x + movement, targetX);
    } else if (pos_x > targetX) {
        pos_x = std::max(pos_x - movement, targetX);
    }

    // Begrenzung auf den Bildschirmrand
    if (pos_x < -1.0f + width) pos_x = -1.0f + width;
    if (pos_x > 1.0f - width) pos_x = 1.0f - width;
}