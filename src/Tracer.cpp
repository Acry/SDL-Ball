#include <epoxy/gl.h>
#include "Tracer.h"

Tracer::Tracer(EventManager *eventMgr) : MovingObject(eventMgr) {
    init();
}

void Tracer::init() {
    len = 100;
    lastX = lastY = 0.0f;
    cr = 1.0f;
    cg = cb = 0.0f;
    width = height = 0.01f;
    color = 0;

    for (int i = 0; i < len; ++i) {
        active[i] = false;
    }
}

void Tracer::update(const float deltaTime) {
    // MovingObject-Update aufrufen für automatische Bewegung
    MovingObject::update(deltaTime);

    // Aktualisiere den Schweif mit aktueller Position
    updatePosition(pos_x, pos_y);
}

void Tracer::draw(const float deltaTime) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < len; i++) {
        if (active[i]) {
            // Stärkere Alpha-Reduktion für ältere Partikel
            a[i] -= (4.0f + i * 0.1f) * deltaTime;

            // Partikel werden dünner mit der Zeit
            s[i] -= 2.0f * deltaTime;
            if (s[i] < 0.2f) s[i] = 0.2f;

            if (a[i] < 0.0f) {
                active[i] = false;
                continue;
            }

            texture->play(deltaTime);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture->textureProperties.texture);
            glLoadIdentity();
            glPushMatrix();
            glTranslatef(x[i], y[i], 0.0);

            // Alpha zusätzlich basierend auf Index reduzieren
            // TODO/FIXME check vertices order, UV coordinates are okay
            const float indexBasedAlpha = a[i] * (1.0f - static_cast<float>(i) / len);
            glColor4f(r[i], g[i], b[i], indexBasedAlpha);

            glBegin(GL_QUADS);

            glTexCoord2f(texture->texturePosition[0], texture->texturePosition[1]);
            glVertex3f(-width * s[i], height * s[i], 0.00);
            glTexCoord2f(texture->texturePosition[2], texture->texturePosition[3]);
            glVertex3f(width * s[i], height * s[i], 0.00);
            glTexCoord2f(texture->texturePosition[4], texture->texturePosition[5]);
            glVertex3f(width * s[i], -height * s[i], 0.00);
            glTexCoord2f(texture->texturePosition[6], texture->texturePosition[7]);
            glVertex3f(-width * s[i], -height * s[i], 0.00);

            glEnd();
            glPopMatrix();
            glDisable(GL_TEXTURE_2D);
        }
    }
    glDisable(GL_BLEND);
}

void Tracer::colorRotate(const bool explosive, const GLfloat c[]) {
    color++;
    if (color > 5)
        color = 0;

    if (!explosive) {
        cr = c[0];
        cg = c[1];
        cb = c[2];
    } else {
        cr = 1.0;
        cg = 0.6;
        cb = 0.0;
    }
}

void Tracer::setSize(GLfloat w, GLfloat h) {
    width = w;
    height = h;
}

void Tracer::updatePosition(const GLfloat nx, const GLfloat ny) {
    // Nur neue Partikel hinzufügen, wenn genügend Abstand
    const GLfloat dist = sqrt(pow(nx - lastX, 2) + pow(ny - lastY, 2));
    if (dist > 0.01) {
        lastX = nx;
        lastY = ny;

        // Freien Platz für neuen Partikel suchen
        for (int i = 0; i < len; i++) {
            if (!active[i]) {
                active[i] = true;
                a[i] = 1.0;
                x[i] = nx;
                y[i] = ny;
                s[i] = 1.0;
                r[i] = cr;
                g[i] = cg;
                b[i] = cb;
                break;
            }
        }
    }
}
