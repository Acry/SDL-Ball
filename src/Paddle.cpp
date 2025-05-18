#include "Paddle.h"
#include <epoxy/gl.h>

Paddle::Paddle() {
    init();
}

void Paddle::init() {
    // GrowableObject-Eigenschaften
    growing = false;
    shrinking = false;
    growSpeed = 0.05f;
    aspectRatio = 0.2f;
    keepAspectRatio = true;

    // GameObject-Eigenschaften (und GrowableObject)
    pos_y = -0.93f;
    pos_x = 0.0f;
    width = 0.059f; // Setzt beide width-Variablen
    height = 0.018f; // Setzt beide height-Variablen

    // Paddle-spezifische Eigenschaften
    dead = false;
    hasGlueLayer = false;
    hasGunLayer = false;
}

void Paddle::grow(const GLfloat targetWidth) {
    setGrowTarget(targetWidth);
}

void Paddle::drawBase() {
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

void Paddle::draw(float deltaTime) {
    if (dead) return;

    // Animation mit deltaTime aktualisieren
    texture.play(deltaTime);

    drawBase();

    if (hasGlueLayer) {
        layerTex[0].play(deltaTime);
        drawGlueLayer();
    }

    if (hasGunLayer) {
        layerTex[1].play(deltaTime);
        drawGunLayer();
    }
}

void Paddle::setGlueLayer(const bool enabled) {
    hasGlueLayer = enabled;
}

void Paddle::setGunLayer(const bool enabled) {
    hasGunLayer = enabled;
}

void Paddle::update(const float deltaTime) {
    // Hier Paddle-Logik implementieren
    updateGrowth(deltaTime);
}

void Paddle::moveTo(const float targetX, const float deltaTime) {
    // Unterscheidung zwischen direkter Positionierung und geschwindigkeitsbasierter Bewegung
    const bool isMouseInput = std::abs(targetX - pos_x) > 0.1f;

    if (isMouseInput) {
        // Mauseingabe: Direkte Positionierung mit Dämpfung
        constexpr float mouseSpeed = 8.0f;
        pos_x += (targetX - pos_x) * mouseSpeed * deltaTime;
    } else {
        // Tastatur/Controller: Konstante Geschwindigkeit
        constexpr float keyboardSpeed = 20.0f;
        // Richtung bestimmen (links/rechts)
        const float direction = (targetX > pos_x) ? 1.0f : -1.0f;
        // Nur bewegen, wenn Differenz vorhanden
        if (targetX != pos_x) {
            // Geschwindigkeitsbasierte Bewegung
            pos_x += direction * keyboardSpeed * deltaTime;

            // Vermeide Überschießen
            if ((direction > 0 && pos_x > targetX) ||
                (direction < 0 && pos_x < targetX)) {
                pos_x = targetX;
            }
        }
    }

    // Begrenzung auf den Bildschirmrand
    if (pos_x < -1.0f + width) pos_x = -1.0f + width;
    if (pos_x > 1.0f - width) pos_x = 1.0f - width;
}
