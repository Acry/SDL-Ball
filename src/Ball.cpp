#include "Ball.h"

#include "colors.h"
#include "game_state.h"
#include "MovingObject.h"
#include "Tracer.h"
#include "Paddle.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "MathHelper.h"

Ball::Ball() {
    // GrowableObject-Eigenschaften initialisieren
    growing = false;
    shrinking = false;
    growSpeed = 0.1f;
    keepAspectRatio = true;  // Ball ist immer rund
    aspectRatio = 1.0f;      // Kreis hat 1:1 Verhältnis

    // MovingObject-Eigenschaften
    width = 0.0f;
    height = 0.0f;

    // Ball-spezifische Eigenschaften
    glued = false;
    pos_x = 0.0f;
    pos_y = 0.0f;
    aimdir = false;
}


void Ball::hit(GLfloat c[]) {
    if (setting.eyeCandy)
        tail.colorRotate(explosive, c);
}

void Ball::move() {
    // Ball Border Collision
    if (pos_x < PLAYFIELD_LEFT_BORDER && xvel < 0.0) {
        soundManager.add(SND_BALL_HIT_BORDER, pos_x);
        xvel *= -1;
    } else if (pos_x + width > PLAYFIELD_RIGHT_BORDER && xvel > 0.0) {
        soundManager.add(SND_BALL_HIT_BORDER, pos_x);
        xvel *= -1;
    } else if (pos_y + height > 1.0f && yvel > 0.0) {
        soundManager.add(SND_BALL_HIT_BORDER, pos_x);
        yvel *= -1;
    } else if (pos_y - height < -1.0f) {
        // paddle y
        active = false;
    }

    pos_x += xvel * globalMilliTicks;

    if (!glued) {
        pos_y += yvel * globalMilliTicks;
    } else {
        gVar.deadTime = 0;
    }

    if (setting.eyeCandy)
        tail.update(pos_x, pos_y);
}

void Ball::draw(const Paddle &paddle) {
    if (setting.eyeCandy)
        tail.draw();

    updateGrowth(globalMilliTicksSinceLastDraw);

    if (glued && player.powerup[PO_LASER]) {
        if (player.powerup[PO_AIM]) {
            if (aimdir == 0) {
                rad -= 1.2f * globalMilliTicksSinceLastDraw;

                if (rad < BALL_MIN_DEGREE)
                    aimdir = true;
            } else {
                rad += 1.2f * globalMilliTicksSinceLastDraw;
                if (rad > BALL_MAX_DEGREE + BALL_MIN_DEGREE)
                    aimdir = false;
            }
            setangle(rad);
        } else {
            getRad();
        }

        const GLfloat bxb = cos(rad) * 0.5f;
        const GLfloat byb = sin(rad) * 0.5f;

        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();
        glTranslatef(pos_x, pos_y, 0.0f);
        glLineWidth(1.0);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINES);
        glColor4f(random_float(2, 0), random_float(1, 0), 0.0, 0.0); // ???
        glVertex3f(0.0, 0.0, 0.0);
        glColor4f(random_float(2, 0), 0.0, 0.0, 1.0);
        glVertex3f(bxb, byb, 0.0);
        glEnd();

        glPointSize(5.0f);
        glColor4f(GL_FULL_RED);
        glEnable(GL_POINT_SMOOTH);
        glBegin(GL_POINTS);
        glVertex3f(bxb, byb, 0.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    if (!glued && player.powerup[PO_AIMHELP]) {
        //Use line intersect to determine if this ball will collide with the paddle

        getRad();
        GLfloat p[4], b[4], o[2]; // Paddle line, ball line, bounce off endpoint
        p[0] = paddle.pos_x - paddle.width;
        p[1] = paddle.pos_x + paddle.width;

        p[2] = paddle.pos_y + paddle.height + height;
        p[3] = paddle.pos_y + paddle.height + height;

        b[0] = pos_x;
        b[1] = pos_x + (cos(rad) * 3.0);
        b[2] = pos_y;
        b[3] = pos_y + (sin(rad) * 3.0);

        GLfloat cx, cy;
        if (LinesCross(p[0], p[2], p[1], p[3], b[0], b[2], b[1], b[3], &cx, &cy)) {
            const GLfloat R = bounceOffAngle(paddle.width, paddle.pos_x, cx);
            o[0] = cx + (cos(R) * 2.0);
            o[1] = cy + (sin(R) * 2.0);
            glDisable(GL_TEXTURE_2D); // ???
            glLineWidth(2.0);
            glEnable(GL_LINE_SMOOTH);
            glLoadIdentity();
            glBegin(GL_LINE_STRIP);
            //Line from ball to paddle
            glColor4f(1.0, 0.0, 0.0, 0.0); // ???
            glVertex3f(b[0], b[2], 0.0);
            glColor4f(1.0, 1.0, 0.0, 1.0);
            glVertex3f(cx, cy, 0.0);
            //Bounce off line.
            glColor4f(1.0, 0.0, 0.0, 0.0); // ???
            glVertex3f(o[0], o[1], 0.0);
            glEnd();
        }
    }

    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glColor4f(GL_WHITE);

    if (explosive) {
        fireTex.play();
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
        texture.play();
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
        glDisable( GL_TEXTURE_2D );
        glColor4f(GL_WHITE);
        glBegin( GL_LINES );
        glVertex3f( -width, height, 0);
        glVertex3f( width, height, 0);

        glVertex3f( -width, -height, 0);
        glVertex3f( width, -height,0);

        glVertex3f( -width, height, 0);
        glVertex3f( -width, -height, 0);

        glVertex3f( width, height, 0);
        glVertex3f( width, -height, 0 );

        glEnd();
        glEnable(GL_TEXTURE_2D);
#endif
}

GLfloat Ball::getRad() {
    rad = atan2(yvel, xvel);
    return (rad);
}

void Ball::setangle(GLfloat o) {
    if (o < BALL_MIN_DEGREE) {
        o = BALL_MIN_DEGREE;
    }

    if (o > BALL_MAX_DEGREE + BALL_MIN_DEGREE) {
        o = BALL_MAX_DEGREE + BALL_MIN_DEGREE;
    }

    rad = o;
    xvel = velocity * cos(rad);
    yvel = velocity * sin(rad);
}

void Ball::setspeed(GLfloat v) {
    if (v > runtime_difficulty.maxballspeed[player.difficulty]) {
        velocity = runtime_difficulty.maxballspeed[player.difficulty];
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

void Ball::checkPaddleCollision(Ball &b, const Paddle &p, position &po) {
    //Er bolden tæt nok på?

    if (b.pos_y < (p.pos_y + p.height) + b.height && b.pos_y > p.pos_y - p.height) {
        if (b.pos_x > p.pos_x - (p.width * 2.0) - b.width && b.pos_x < p.pos_x + (p.width * 2.0) + b.width) {
            GLfloat py = 0;
            GLfloat px = 0;
            bool col = false;
            int points = 0;
            for (int i = 0; i < 32; i++) {
                const GLfloat x = b.bsin[i];
                const GLfloat y = b.bcos[i];

                //Find de punkter der er inden i padden
                if (b.pos_x + x > p.pos_x - p.width && b.pos_x + x < p.pos_x + p.width) {
                    if (b.pos_y + y < p.pos_y + p.height && b.pos_y + y > p.pos_y - p.height) {
                        col = true;

                        px += x;
                        py += y;
                        points++;
                    }
                }
            } //For loop

            if (col) {
                col = false;
                gVar.deadTime = 0;
                px /= static_cast<float>(points);
                py /= static_cast<float>(points);

                px = b.pos_x + px;

                //Ved at reagere herinde fungerer yvel som en switch, så det kun sker een gang ;)
                if (b.yvel < 0) {
                    b.pos_y = p.pos_y + p.height + b.height; //løft op over pad

                    //Only decrease speed if the player does not have the go-thru powerup
                    if (!player.powerup[PO_THRU]) {
                        b.setspeed(b.velocity + runtime_difficulty.hitpaddleinc[player.difficulty]);
                    }

                    b.setangle(bounceOffAngle(p.width, p.pos_x, b.pos_x));
                    if (player.powerup[PO_GLUE]) {
                        b.gluedX = p.pos_x + p.width - px;
                        b.glued = true;
                    }

                    po.x = px;
                    po.y = py;
                }
            }
        }
    }
}

float Ball::bounceOffAngle(const GLfloat width, GLfloat posx, GLfloat hitx) {
    // Berechnet den Abprallwinkel basierend auf der Auftreffposition
    GLfloat relativeX = (hitx - posx) / width;  // Position relativ zur Paddle-Mitte

    // Begrenze den Winkel auf den erlaubten Bereich
    if (relativeX > 1.0f) relativeX = 1.0f;
    if (relativeX < -1.0f) relativeX = -1.0f;

    // Berechne den Winkel zwischen BALL_MIN_DEGREE und BALL_MAX_DEGREE
    return BALL_MIN_DEGREE + ((BALL_MAX_DEGREE) * (relativeX + 1.0f) / 2.0f);
}
//static float bounceOffAngle(const GLfloat width, GLfloat posx, GLfloat hitx) {
//    return BALL_MAX_DEGREE / (width * 2.0f) * (posx + width - hitx) + BALL_MIN_DEGREE;
//}

void Ball::onSizeChanged() {
    // Diese Methode wird aufgerufen, wenn die Größe geändert wurde
    // Aktualisiere die Tracer-Größe
    tail.width = width;
    tail.height = height;

    // Aktualisiere die Punkte für den Ball (bsin und bcos Arrays)
    int i = 0;
    for (float rad = 0.0; rad < 6.3; rad += 0.2) {
        if (i < 32) {
            bsin[i] = sin(rad) * width;
            bcos[i] = cos(rad) * width;
        }
        i++;
    }
}