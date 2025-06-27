// Particle.cpp
#include "Particle.h"
#include <epoxy/gl.h>

Particle::Particle(const TextureResource &tex) {
    effectProperties.texture = tex;
}

void Particle::update(const float deltaTime) {
    if (lifeLeft > 0) {
        lifeLeft -= static_cast<int>(deltaTime * 1000.0f);

        // Alpha-Wert basierend auf verbleibender Lebensdauer berechnen
        const float originalAlpha = effectProperties.color[3];
        effectProperties.color[3] = static_cast<float>(lifeLeft) / static_cast<float>(life) * originalAlpha;

        // Bewegungsphysik
        v.y -= effectProperties.gravity * deltaTime;
        v.y -= bounce * deltaTime;

        if (v.x < 0) v.x += f * deltaTime;
        else v.x -= f * deltaTime;

        p.x += v.x * deltaTime;
        p.y += v.y * deltaTime;
    } else {
        active = false;
    }
}

void Particle::draw() const {
    if (!active) return;
    const float curSize = size / static_cast<float>(life) * static_cast<float>(lifeLeft);

    glColor4f(effectProperties.color[0], effectProperties.color[1], effectProperties.color[2],
              effectProperties.color[3]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, effectProperties.texture.id);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(p.x - curSize, p.y + curSize, 0.0f);
    glTexCoord2f(0, 1);
    glVertex3f(p.x + curSize, p.y + curSize, 0.0f);
    glTexCoord2f(1, 1);
    glVertex3f(p.x + curSize, p.y - curSize, 0.0f);
    glTexCoord2f(1, 0);
    glVertex3f(p.x - curSize, p.y - curSize, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
