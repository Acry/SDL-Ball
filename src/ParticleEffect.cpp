// ParticleEffect.cpp
#include "ParticleEffect.h"

void ParticleEffect::init(const position &pos) {
    spawnPos = pos;
    particles.clear();
    particles.resize(properties.count);

    if (properties.speed == 0.0f) {
        properties.speed = 0.5f;
    }

    for (int i = 0; i < properties.count; ++i) {
        auto &particle = particles[i];
        particle.size = randomFloat(properties.size, 0.0f);
        particle.life = randomInt(1, properties.life);
        particle.lifeLeft = particle.life;

        // Richtungswinkel basierend auf directionAngle und directionVariance berechnen
        const float angleVariance = randomFloat(properties.directionVariance, -properties.directionVariance);
        const float angle = (properties.directionAngle + angleVariance) * (PI_F / 180.0f);

        // Geschwindigkeit mit zufälliger Variation
        const float speedVariation = randomFloat(properties.speed * 2.0f, properties.speed * 0.5f);
        particle.v.x = speedVariation * sin(angle);
        particle.v.y = speedVariation * cos(angle);

        particle.bounce = randomFloat(0.01f, 0.0f);
        particle.f = randomFloat(0.02f, 0.0f);

        particle.effectProperties = properties;
        particle.effectProperties.texture = properties.texture;

        // Wenn eine Farbpalette verfügbar ist, zufällige Farbe auswählen
        if (properties.useColorPalette && !properties.colorPalette.empty()) {
            int colorIndex = randomInt(0, properties.colorPalette.size() - 1);
            particle.effectProperties.color = properties.colorPalette[colorIndex];
        }

        particle.p = pos;
        particle.active = true;
    }

    properties.active = true;
}

void ParticleEffect::update(const float deltaTime) {
    bool anyActive = false;
    for (auto &s: particles) {
        if (s.active) {
            s.update(deltaTime);
            if (s.active) anyActive = true;
        }
    }
    properties.active = anyActive;
}

void ParticleEffect::draw() {
    for (const auto &s: particles) {
        if (s.active) s.draw();
    }
}

bool ParticleEffect::isActive() const {
    return properties.active;
}
