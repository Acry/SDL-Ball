#include "Paddle.h"
#include <GL/gl.h>

Paddle::Paddle(const texture &tex) : GrowableObject(tex) {
    this->width = 0.124f;
    this->height = 0.032f;
    this->velocity = 0.0f;
    pos_y = -0.955f;
    pos_x = 0.0f - width / 2.0f;
    aspectRatio = width / height;
    onSizeChanged();
}

void Paddle::init() {
    // Implementierung der abstrakten Methode aus GameObject
}

void Paddle::draw() const {
    drawBase();
    if (hasGlueLayer) {
        drawGlueLayer();
    }
    if (hasGunLayer) {
        drawGunLayer();
    }
}

void Paddle::update(float deltaTime) {
    GrowableObject::update(deltaTime);
    centerX = pos_x + width / 2.0f;
    centerY = pos_y + height / 2.0f;
    collisionPoints = *getCollisionPoints();
}

void Paddle::setActive(bool value) { collisionActive = value; }
float Paddle::getPosX() const { return pos_x; }
float Paddle::getPosY() const { return pos_y; }
float Paddle::getWidth() const { return width; }
float Paddle::getHeight() const { return height; }
bool Paddle::isActive() const { return collisionActive; }
CollisionType Paddle::getCollisionType() const { return CollisionType::Ball; }

void Paddle::drawBase() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, textureProperties.id);
    glColor4fv(textureProperties.textureColor);
    glBegin(GL_QUADS);

    glTexCoord2f(textureProperties.uvCoordinates[0], textureProperties.uvCoordinates[1]);
    glVertex3f(pos_x, pos_y, 0.0f);

    glTexCoord2f(textureProperties.uvCoordinates[2], textureProperties.uvCoordinates[3]);
    glVertex3f(pos_x + width, pos_y, 0.0f);

    glTexCoord2f(textureProperties.uvCoordinates[4], textureProperties.uvCoordinates[5]);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);

    glTexCoord2f(textureProperties.uvCoordinates[6], textureProperties.uvCoordinates[7]);
    glVertex3f(pos_x, pos_y + height, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Paddle::drawGlueLayer() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, glueLayerTextureProperties.id);
    glColor4fv(glueLayerTextureProperties.textureColor);
    glBegin(GL_QUADS);

    glTexCoord2f(glueLayerTextureProperties.uvCoordinates[0], glueLayerTextureProperties.uvCoordinates[1]);
    glVertex3f(pos_x, pos_y, 0.0f);

    glTexCoord2f(glueLayerTextureProperties.uvCoordinates[2], glueLayerTextureProperties.uvCoordinates[3]);
    glVertex3f(pos_x + width, pos_y, 0.0f);

    glTexCoord2f(glueLayerTextureProperties.uvCoordinates[4], glueLayerTextureProperties.uvCoordinates[5]);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);

    glTexCoord2f(glueLayerTextureProperties.uvCoordinates[6], glueLayerTextureProperties.uvCoordinates[7]);
    glVertex3f(pos_x, pos_y + height, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Paddle::drawGunLayer() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, gunLayerTextureProperties.id);
    glColor4fv(gunLayerTextureProperties.textureColor);
    glBegin(GL_QUADS);

    glTexCoord2f(gunLayerTextureProperties.uvCoordinates[0], gunLayerTextureProperties.uvCoordinates[1]);
    glVertex3f(pos_x, pos_y + height, 0.0f);

    glTexCoord2f(gunLayerTextureProperties.uvCoordinates[2], gunLayerTextureProperties.uvCoordinates[3]);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);

    glTexCoord2f(gunLayerTextureProperties.uvCoordinates[4], gunLayerTextureProperties.uvCoordinates[5]);
    glVertex3f(pos_x + width, pos_y + height + height / 3.0f, 0.0f);

    glTexCoord2f(gunLayerTextureProperties.uvCoordinates[6], gunLayerTextureProperties.uvCoordinates[7]);
    glVertex3f(pos_x, pos_y + height + height / 3.0f, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Paddle::setGlueLayer(bool enabled) { hasGlueLayer = enabled; }
bool Paddle::getGlueLayer() const { return hasGlueLayer; }
void Paddle::setGunLayer(bool enabled) { hasGunLayer = enabled; }
bool Paddle::getGunLayer() const { return hasGunLayer; }

std::vector<float> *Paddle::getCollisionPoints() const {
    static std::vector<float> points;
    points.clear();
    points.reserve(64);

    const float centerX = pos_x + width / 2.0f;
    const float centerY = pos_y + height / 2.0f;

    for (int i = 0; i < 32; ++i) {
        const float angle = i * (2.0f * M_PI / 32.0f);
        const float pointX = centerX + std::cos(angle) * (width / 2.0f);
        const float pointY = centerY + std::sin(angle) * (height / 2.0f);
        points.push_back(pointX);
        points.push_back(pointY);
    }
    return &points;
}

void Paddle::onSizeChanged() {
    //collisionPoints = *getCollisionPoints();
}

bool Paddle::isPhysicallyActive() const { return active; }
void Paddle::setPhysicallyActive(bool value) { active = value; }
