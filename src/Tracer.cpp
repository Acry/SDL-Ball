#include <epoxy/gl.h>
#include "Tracer.h"

Tracer::Tracer(const texture &tex) : MovingObject(tex) {
    init();
}

void Tracer::init() {
    len = 100;
    lastX = lastY = 0.0f;
    cr = 1.0f;
    cg = cb = 0.0f;
    width = height = 1.0f;
    color = 0;

    for (int i = 0; i < len; ++i) {
        active[i] = false;
    }
}

void Tracer::update(const float deltaTime) {
    MovingObject::update(deltaTime);
    if (m_rainbowEnabled) {
        rainbowColorUpdate(deltaTime);
    }
    updatePosition(pos_x, pos_y);
    updateParticles(deltaTime);
}

void Tracer::draw() const {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureProperties.id);
    glLoadIdentity();

    for (int i = 0; i < len; i++) {
        if (active[i]) {
            const float halfWidth = width * s[i] * 0.5f;
            const float halfHeight = height * s[i] * 0.5f;
            const float indexBasedAlpha = a[i] * (1.0f - static_cast<float>(i) / len);
            glColor4f(r[i], g[i], b[i], indexBasedAlpha);
            glBegin(GL_QUADS);

            // Links oben
            glTexCoord2f(textureProperties.uvCoordinates[0], textureProperties.uvCoordinates[1]);
            glVertex3f(x[i] - halfWidth, y[i] + halfHeight, 0.0f);

            // Rechts oben
            glTexCoord2f(textureProperties.uvCoordinates[2], textureProperties.uvCoordinates[3]);
            glVertex3f(x[i] + halfWidth, y[i] + halfHeight, 0.0f);

            // Rechts unten
            glTexCoord2f(textureProperties.uvCoordinates[4], textureProperties.uvCoordinates[5]);
            glVertex3f(x[i] + halfWidth, y[i] - halfHeight, 0.0f);

            // Links unten
            glTexCoord2f(textureProperties.uvCoordinates[6], textureProperties.uvCoordinates[7]);
            glVertex3f(x[i] - halfWidth, y[i] - halfHeight, 0.0f);

            glEnd();
        }
    }
    glDisable(GL_TEXTURE_2D);
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

void Tracer::setSize(const GLfloat factor) {
    width *= factor;
    height *= factor;
}

void Tracer::updatePosition(const GLfloat nx, const GLfloat ny) {
    pos_x = nx;
    pos_y = ny;
    const GLfloat dist = sqrtf(powf(nx - lastX, 2) + pow(ny - lastY, 2));
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

void Tracer::updateParticles(const float deltaTime) {
    for (int i = 0; i < len; i++) {
        if (active[i]) {
            // Stärkere Alpha-Reduktion für ältere Partikel
            a[i] -= (4.0f + i * 0.1f) * deltaTime;

            // Partikel werden dünner mit der Zeit
            s[i] -= 2.0f * deltaTime;
            if (s[i] < 0.2f) s[i] = 0.2f;

            if (a[i] < 0.0f) {
                active[i] = false;
            }
        }
    }
}

void Tracer::setLastPosition(const GLfloat nx, const GLfloat ny) {
    lastX = nx;
    lastY = ny;
}

void Tracer::rainbowColorUpdate(const float deltaTime) {
    static float hue = 0.0f;
    hue += deltaTime * 0.5f; // Geschwindigkeit der Farbänderung
    if (hue > 1.0f) hue -= 1.0f;

    // HSV zu RGB Konvertierung
    float h = hue * 6.0f;
    float s = 1.0f; // Sättigung
    float v = 1.0f; // Helligkeit
    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
    float m = v - c;

    // Standardwerte setzen
    cr = cg = cb = 0.0f;

    // Farbe basierend auf HSV-Wert festlegen
    if (h < 1.0f) {
        cr = c;
        cg = x;
    } else if (h < 2.0f) {
        cr = x;
        cg = c;
    } else if (h < 3.0f) {
        cg = c;
        cb = x;
    } else if (h < 4.0f) {
        cg = x;
        cb = c;
    } else if (h < 5.0f) {
        cr = x;
        cb = c;
    } else {
        cr = c;
        cb = x;
    }

    // Helligkeit anpassen
    cr += m;
    cg += m;
    cb += m;

    // Farbe sofort auf aktive Partikel anwenden (optional)
    for (int i = 0; i < len; i++) {
        if (active[i]) {
            r[i] = cr;
            g[i] = cg;
            b[i] = cb;
        }
    }
}

void Tracer::enableRainbow(const bool enable) {
    m_rainbowEnabled = enable;
}

bool Tracer::isRainbowEnabled() const {
    return m_rainbowEnabled;
}
