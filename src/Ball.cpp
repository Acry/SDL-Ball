#include "Ball.h"
#include "config.h"

Ball::Ball(EventManager *eventMgr) : eventManager(eventMgr) {
    this->eventManager = eventMgr;
    init();
}

void Ball::init() {
    // GameObject
    width = 0.03f;
    height = 0.03f;
    active = true;
    pos_x = 0.0f;
    pos_y = 0.0f;

    // MovingObject
    velocity = 0.3f;
    xvel = 0.0f;
    yvel = 0.0f;

    // Ball-specific
    glued = true;
    explosive = false;

    // Collision
    onSizeChanged();
}

void Ball::draw(const float deltaTime) {
    if (!active) return;

    if (explosive) {
        fireTex->play(deltaTime);
        drawExplosiveLayer();
        return;
    }
    texture->play(deltaTime);
    drawBase();

#if 0
} else {
    // Normaler Ball hat momentan keine Animation
    // texture->play(deltaTime); // ---FIXME
    glBindTexture(GL_TEXTURE_2D, texture->textureProperties.texture);
    //glColor4f(texture->textureProperties.glTexColorInfo[0], texture->textureProperties.glTexColorInfo[1],
    //          texture->textureProperties.glTexColorInfo[2],
    //          texture->textureProperties.glTexColorInfo[3]);
    // Von -1 bis +1 in NDC, direkt Stein an Stein
    // glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection
    //         +1
    //         ^
    //         |
    // -1 <----+----> +1
    //         |
    //        -1
    glBegin(GL_QUADS);
    // (0, 0): Bottom-left corner.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(pos_x, pos_y, 0.0f);
    // (1, 0): Bottom-right corner.
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(pos_x + width, pos_y, 0.0f);
    // (1, 1): Top-right corner.
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);
    // (0, 1): Top-left corner.
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(pos_x, pos_y + height, 0.0f);
    // In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
    // (0, 1) ------ ( 1, 1)
    //   |              |
    //   |              |
    //   |              |
    //   (0, 0) ------ (1, 0)
    glEnd();
}
#endif
}

void Ball::drawBase() const {
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

void Ball::drawExplosiveLayer() const {
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fireTex->textureProperties.texture);
    glColor4f(fireTex->textureProperties.glTexColorInfo[0], fireTex->textureProperties.glTexColorInfo[1],
              fireTex->textureProperties.glTexColorInfo[2],
              fireTex->textureProperties.glTexColorInfo[3]);

    glBegin(GL_QUADS);

    // Bottom-left corner.
    glTexCoord2f(fireTex->texturePosition[0], fireTex->texturePosition[1]);
    glVertex3f(pos_x, pos_y, 0.0f);

    // Bottom-right corner.
    glTexCoord2f(fireTex->texturePosition[2], fireTex->texturePosition[3]);
    glVertex3f(pos_x + width, pos_y, 0.0f);

    // Top-right corner.
    glTexCoord2f(fireTex->texturePosition[4], fireTex->texturePosition[5]);
    glVertex3f(pos_x + width, pos_y + height, 0.0f);

    // Top-left corner.
    glTexCoord2f(fireTex->texturePosition[6], fireTex->texturePosition[7]);
    glVertex3f(pos_x, pos_y + height, 0.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

GLfloat Ball::getAngle() {
    rad = atan2(yvel, xvel);
    return (rad);
}

void Ball::setAngle(GLfloat o) {
    if (o < MIN_BOUNCE_ANGLE) {
        o = MIN_BOUNCE_ANGLE;
    }

    if (o > BOUNCE_ANGLE_RANGE + MIN_BOUNCE_ANGLE) {
        o = BOUNCE_ANGLE_RANGE + MIN_BOUNCE_ANGLE;
    }

    rad = o;
    xvel = velocity * cos(rad);
    yvel = velocity * sin(rad);
}

void Ball::setSpeed(const GLfloat v, const GLfloat maxSpeed) {
    if (v > maxSpeed) {
        velocity = maxSpeed;
    } else {
        velocity = v;
    }

    getAngle();
    xvel = velocity * cos(rad);
    yvel = velocity * sin(rad);
}

void Ball::setSize(GLfloat s) {
    // Verwende die GrowableObject-Implementierung
    setGrowTarget(s);

    // Direkte Aktualisierung der Kollisionspunkte
    onSizeChanged();
}

void Ball::update(const float deltaTime) {
    if (!active) return;
    if (growing || shrinking) {
        centerX = pos_x + width / 2.0f;
        centerY = pos_y + height / 2.0f;
        updateGrowth(deltaTime);
    }

    if (!glued) {
        EventData data;
        data.posX = pos_x;
        data.posY = pos_y;
        data.sender = this;
        data.target = nullptr;
        data.points = explosive ? 1 : 0;
        eventManager->emit(GameEvent::BallMoved, data);
        MovingObject::update(deltaTime);
    } else {
        pos_x += xvel * deltaTime;
    }
}

void Ball::onSizeChanged() {
    pos_x = centerX - width / 2.0f;
    pos_y = centerY - height / 2.0f;
    // Aktualisiere die Punkte für den Ball (bsin und bcos Arrays)
    // constexpr int POINTS = 32;
    // for (int i = 0; i < POINTS; ++i) {
    //     constexpr float TWO_PI = 6.28318531f;
    //     // Berechne den Winkel basierend auf dem Index
    //     const float rad = static_cast<float>(i) * TWO_PI / POINTS;
    //     bsin[i] = sin(rad) * width;
    //     bcos[i] = cos(rad) * width;
    // }
}

void Ball::launchFromPaddle() {
    glued = false;

    // Setze einen Standardwinkel für den Start (nach oben gerichtet)
    // Mit leichter Abweichung für besseres Spielgefühl
    const float launchAngle = M_PI / 2 + (rand() % 40 - 20) * 0.01f; // π/2 ± kleine Zufallsabweichung

    // Aktualisiere xvel und yvel basierend auf velocity und dem Winkel
    xvel = velocity * cos(launchAngle);
    yvel = velocity * sin(launchAngle);

    // Speichere den Winkel für zukünftige Berechnungen
    rad = launchAngle;
}

const std::vector<float> *Ball::getCollisionPoints() const {
    // Hier könntest du ein statisches Array mit den bsin/bcos-Werten zurückgeben
    // für eine genauere Kreis-Kollisionsberechnung
    static std::vector<float> points;
    if (points.empty()) {
        points.reserve(64); // 32 Punkte * 2 Koordinaten
        for (int i = 0; i < 32; ++i) {
            points.push_back(pos_x + bcos[i]);
            points.push_back(pos_y + bsin[i]);
        }
    }
    return &points;
}

void Ball::onCollision(ICollideable *other, float hitX, float hitY) {
    if (!eventManager) return;

    EventData data;
    data.posX = hitX;
    data.posY = hitY;
    data.sender = this;
    data.target = dynamic_cast<const GameObject*>(other);

    switch (other->getCollisionType()) {
        case static_cast<int>(CollisionType::Paddle):
            eventManager->emit(GameEvent::BallHitPaddle, data);
            break;
        case static_cast<int>(CollisionType::Brick):
            eventManager->emit(GameEvent::BallHitBrick, data);
            break;
        case static_cast<int>(CollisionType::BorderLeft):
            eventManager->emit(GameEvent::BallHitLeftBorder, data);
            break;
        case static_cast<int>(CollisionType::BorderRight):
            eventManager->emit(GameEvent::BallHitRightBorder, data);
            break;
        case static_cast<int>(CollisionType::BorderTop):
            eventManager->emit(GameEvent::BallHitTopBorder, data);
            break;
        default: break;
    }
}

int Ball::getCollisionType() const {
    return static_cast<int>(CollisionType::Ball);
}

Ball::~Ball() {
    EventData data;
    data.sender = this;
    eventManager->emit(GameEvent::BallDestroyed, data);
}
