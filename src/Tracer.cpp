#include <epoxy/gl.h>
#include "Tracer.h"
#include "SpriteSheetAnimation.h"

Tracer::Tracer() {
    len = 100;
    lastX = lastY = 0;
    cr = 1;
    cg = cb = 0;
    height = width = 0.01;
    for (bool &i: active) i = false;
}

void Tracer::draw(float deltaTime) {
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

            tex.play(deltaTime);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
            glLoadIdentity();
            glTranslatef(x[i], y[i], 0.0);

            // Alpha zusätzlich basierend auf Index reduzieren
            float indexBasedAlpha = a[i] * (1.0f - static_cast<float>(i) / len);
            glColor4f(r[i], g[i], b[i], indexBasedAlpha);

            glBegin(GL_QUADS);
            glTexCoord2f(tex.texturePosition[0], tex.texturePosition[1]);
            glVertex3f(-width * s[i], height * s[i], 0.00);
            glTexCoord2f(tex.texturePosition[2], tex.texturePosition[3]);
            glVertex3f(width * s[i], height * s[i], 0.00);
            glTexCoord2f(tex.texturePosition[4], tex.texturePosition[5]);
            glVertex3f(width * s[i], -height * s[i], 0.00);
            glTexCoord2f(tex.texturePosition[6], tex.texturePosition[7]);
            glVertex3f(-width * s[i], -height * s[i], 0.00);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }
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

void Tracer::update(const GLfloat nx, const GLfloat ny) {
    // If long enough away
    const GLfloat dist = sqrt(pow(nx - lastX, 2) + pow(ny - lastY, 2));
    if (dist > 0.01) {
        lastX = nx;
        lastY = ny;
        //find a non-active trail-part
        for (int i = 0; i < len; i++) {
            if (!active[i]) {
                active[i] = true;
                a[i] = 1.0; //tweak me
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