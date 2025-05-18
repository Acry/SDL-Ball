#include "Ball.h"

#include "colors.h"
#include "config.h"
#include "Tracer.h"
#include "SettingsManager.h"

Ball::Ball(EventManager* eventMgr) : eventManager(eventMgr) {
 init();
}

void Ball::init() {
    // MovingObject-Eigenschaften
    width = 0.0f;
    height = 0.0f;
    eyeCandy = true;
    // Ball-spezifische Eigenschaften
    glued = false;
    pos_x = 0.0f;
    pos_y = 0.0f;
    aimdir = false;

    // GrowableObject-Eigenschaften neu initialisieren
    growing = false;
    shrinking = false;
    growSpeed = 0.1f;
    keepAspectRatio = true;
    aspectRatio = 1.0f;
}

void Ball::hit(GLfloat c[]) {
    if (eyeCandy)
        tail.colorRotate(explosive, c);
}

void Ball::update(float deltaTime) {
    // Ball Border Collision
    if (pos_x < PLAYFIELD_LEFT_BORDER && xvel < 0.0) {
        // Event auslösen statt direktem soundManager-Aufruf
        EventData data;
        data.posX = pos_x;
        data.soundID = SND_BALL_HIT_BORDER;
        eventManager->emit(GameEvent::BallHitBorder, data);

        xvel *= -1;
    } else if (pos_x + width > PLAYFIELD_RIGHT_BORDER && xvel > 0.0) {
        // Event auslösen
        EventData data;
        data.posX = pos_x;
        data.soundID = SND_BALL_HIT_BORDER;
        eventManager->emit(GameEvent::BallHitBorder, data);

        xvel *= -1;
    } else if (pos_y + height > 1.0f && yvel > 0.0) {
        // Event auslösen
        EventData data;
        data.posX = pos_x;
        data.soundID = SND_BALL_HIT_BORDER;
        eventManager->emit(GameEvent::BallHitBorder, data);

        yvel *= -1;
    } else if (pos_y - height < -1.0f) {
        // Event für verlorenen Ball auslösen
        EventData data;
        data.posX = pos_x;
        data.posY = pos_y;
        eventManager->emit(GameEvent::BallLost, data);

        active = false;
    }

    pos_x += xvel * deltaTime;

    if (!glued) {
        pos_y += yvel * deltaTime;
    } else {
        // Globale Variable entfernen
        // gVar.deadTime = 0;
    }

    if (eyeCandy)
        tail.update(pos_x, pos_y);
}

void Ball::draw(float deltaTime) {
    if (eyeCandy)
        tail.draw();

    updateGrowth(deltaTime);

    // Paddle-spezifischer Code entfernt
    // Funktionalität für Zielhilfe sollte in eine separate Klasse/Manager ausgelagert werden

    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glColor4f(GL_WHITE);

    if (explosive) {
        fireTex.play(deltaTime);
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
        texture.play(deltaTime);
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
}

GLfloat Ball::getRad() {
    rad = atan2(yvel, xvel);
    return (rad);
}

void Ball::setangle(GLfloat o) {
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

void Ball::setspeed(GLfloat v) {
    // Konstanten für Konfigurationswerte verwenden
    static constexpr GLfloat DEFAULT_MAX_BALL_SPEED = 0.5f;

    // Globale Variablen entfernen und durch lokale Parameter ersetzen
    GLfloat maxSpeed = DEFAULT_MAX_BALL_SPEED;

    if (v > maxSpeed) {
        velocity = maxSpeed;
    } else {
        velocity = v;
    }

    getRad();
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
    // Diese Methode wird aufgerufen, wenn die Größe geändert wurde
    // Aktualisiere die Tracer-Größe
    tail.width = width;
    tail.height = height;

    // Aktualisiere die Punkte für den Ball (bsin und bcos Arrays)
    constexpr int POINTS = 32;
    constexpr float TWO_PI = 6.28318531f;  // Verwende die RAD-Konstante

    // Verwende einen Integer-Index für die Schleife
    for (int i = 0; i < POINTS; ++i) {
        // Berechne den Winkel basierend auf dem Index
        float rad = static_cast<float>(i) * TWO_PI / POINTS;
        bsin[i] = sin(rad) * width;
        bcos[i] = cos(rad) * width;
    }
}
