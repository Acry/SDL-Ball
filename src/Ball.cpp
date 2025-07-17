#include "Ball.h"
#include <GL/gl.h>

Ball::Ball(const texture &tex) : GrowableObject(tex) {
    this->width = 0.03f;
    this->height = 0.03f;
    this->velocity = 0.5f;
    onSizeChanged();
}

void Ball::init() {
    // Implementierung der abstrakten Methode aus GameObject
}

void Ball::draw() const {
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

void Ball::update(float deltaTime) {
    GrowableObject::update(deltaTime);
    centerX = pos_x + width / 2.0f;
    centerY = pos_y + height / 2.0f;
    collisionPoints = *getCollisionPoints();
}

void Ball::setActive(bool value) { collisionActive = value; }
float Ball::getPosX() const { return pos_x; }
float Ball::getPosY() const { return pos_y; }
float Ball::getWidth() const { return width; }
float Ball::getHeight() const { return height; }
bool Ball::isActive() const { return collisionActive; }

CollisionType Ball::getCollisionType() const {
    return CollisionType::Ball;
}

float Ball::getAngle() const {
    return angleRadians;
}

void Ball::setAngle(float o) {
    angleRadians = o;
    xvel = velocity * std::cos(angleRadians);
    yvel = velocity * std::sin(angleRadians);
}

void Ball::setSpeed(float v) {
    velocity = v;
    xvel = velocity * std::cos(angleRadians);
    yvel = velocity * std::sin(angleRadians);
}

void Ball::setSize(float s) {
    setGrowTarget(s);
}

std::vector<float> *Ball::getCollisionPoints() const {
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

void Ball::onSizeChanged() {
    //collisionPoints = *getCollisionPoints();
}

bool Ball::isPhysicallyActive() const { return active; }
void Ball::setPhysicallyActive(bool value) { active = value; }
bool Ball::isGlued() const { return glued; }
void Ball::setGlued(bool value) { glued = value; }
bool Ball::isExplosive() const { return explosive; }
void Ball::setExplosive(bool value) { explosive = value; }
