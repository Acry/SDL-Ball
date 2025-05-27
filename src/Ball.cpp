#include "Ball.h"

#include "colors.h"
#include "config.h"

Ball::Ball(EventManager *eventMgr) : eventManager(eventMgr) {
    init();
}

// Eigentschaften sortieren, sollten wie beim Paddle aussehen
void Ball::init() {
    // MovingObject-Eigenschaften mit sinnvollen Standardwerten
    width = 0.018f;
    height = 0.018f;
    active = true; // Ball ist standardmäßig aktiv

    // Bewegungseigenschaften
    velocity = 0.3f; // Standardgeschwindigkeit
    xvel = 0.0f;
    yvel = 0.0f;

    // Ball-spezifische Eigenschaften
    // Nur wahr für Bälle die nicht im Feld spawnen
    glued = true; // Ball startet am Paddle festgeklebt
    pos_x = 0.0f;
    pos_y = 0.0f;
    aimdir = false;
    explosive = false;
    gluedX = 0.0f;

    // GrowableObject-Eigenschaften neu initialisieren
    growing = false;
    shrinking = false;
    growSpeed = 0.05f;
    keepAspectRatio = true;
    aspectRatio = 1.0f;

    // Kollisionspunkte initialisieren
    onSizeChanged();
}

void Ball::update(float deltaTime) {
    updateGrowth(deltaTime);

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

// ---FIXME: Ball und Paddle sollten sehr ähnlich sein
// drawFireBall
// drawNormalBall / DrawBase ?
void Ball::draw(const float deltaTime) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Funktionalität für Zielhilfe sollte in eine separate Klasse/Manager ausgelagert werden

    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glColor4f(GL_WHITE);
    if (explosive) {
        fireTex.play(deltaTime / 5.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fireTex.textureProperties.texture);
        glColor4f(fireTex.textureProperties.glTexColorInfo[0], fireTex.textureProperties.glTexColorInfo[1],
                  fireTex.textureProperties.glTexColorInfo[2],
                  fireTex.textureProperties.glTexColorInfo[3]);
        glBegin(GL_QUADS);
        glTexCoord2f(fireTex.texturePosition[0], fireTex.texturePosition[1]);
        glVertex3f(-width, height, 0.0);
        glTexCoord2f(fireTex.texturePosition[2], fireTex.texturePosition[3]);
        glVertex3f(width, height, 0.0);
        glTexCoord2f(fireTex.texturePosition[4], fireTex.texturePosition[5]);
        glVertex3f(width, -height, 0.0);
        glTexCoord2f(fireTex.texturePosition[6], fireTex.texturePosition[7]);
        glVertex3f(-width, -height, 0.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        // Normaler Ball hat momentan keine Animation
        texture.play(deltaTime); // ---FIXME
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
        glColor4f(texture.textureProperties.glTexColorInfo[0], texture.textureProperties.glTexColorInfo[1],
                  texture.textureProperties.glTexColorInfo[2],
                  texture.textureProperties.glTexColorInfo[3]);
        glBegin(GL_QUADS);
        glTexCoord2f(texture.texturePosition[0], texture.texturePosition[1]);
        glVertex3f(-width, height, 0.0);
        glTexCoord2f(texture.texturePosition[2], texture.texturePosition[3]);
        glVertex3f(width, height, 0.0);
        glTexCoord2f(texture.texturePosition[4], texture.texturePosition[5]);
        glVertex3f(width, -height, 0.0);
        glTexCoord2f(texture.texturePosition[6], texture.texturePosition[7]);
        glVertex3f(-width, -height, 0.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

#if DEBUG_DRAW_BALL_QUAD
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glDisable(GL_TEXTURE_2D);
    glColor4f(GL_WHITE);
    glBegin(GL_LINES);
    glVertex3f(-width, height, 0);
    glVertex3f(width, height, 0);

    glVertex3f(-width, -height, 0);
    glVertex3f(width, -height, 0);

    glVertex3f(-width, height, 0);
    glVertex3f(-width, -height, 0);

    glVertex3f(width, height, 0);
    glVertex3f(width, -height, 0);

    glEnd();
    glEnable(GL_TEXTURE_2D);
#endif
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

void Ball::onSizeChanged() {
    // Aktualisiere die Punkte für den Ball (bsin und bcos Arrays)
    constexpr int POINTS = 32;
    for (int i = 0; i < POINTS; ++i) {
        constexpr float TWO_PI = 6.28318531f;
        // Berechne den Winkel basierend auf dem Index
        const float rad = static_cast<float>(i) * TWO_PI / POINTS;
        bsin[i] = sin(rad) * width;
        bcos[i] = cos(rad) * width;
    }
}

void Ball::launchFromPaddle() {
    glued = false;
    setAngle(RAD / 4); // Standard-Winkel von 45°
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
    data.target = other;

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
