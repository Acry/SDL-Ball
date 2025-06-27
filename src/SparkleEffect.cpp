// SparkleEffect.cpp
#include "SparkleEffect.h"

SparkleEffect::SparkleEffect() = default;

SparkleEffect::~SparkleEffect() = default;

void SparkleEffect::init(const position &pos) {
    spawnPos = pos;
    particles.clear();
    particles.resize(properties.num);

    if (properties.speed == 0.0f) {
        properties.speed = 0.5f;
    }
    for (int i = 0; i < properties.num; ++i) {
        auto &particle = particles[i];
        particle.size = randomFloat(properties.size, 0.0f);
        particle.life = randomInt(1, properties.life);
        particle.lifeLeft = particle.life;

        const float angle = randomFloat(360.0f, 0.0f) * (PI_F / 180.0f); // Zufälliger Winkel in Radiant
        particle.v.x = (properties.speed * randomFloat(properties.speed * 2.0f, 0.0f)) * sin(angle);
        particle.v.y = (properties.speed * randomFloat(properties.speed * 2.0f, 0.0f)) * cos(angle);

        particle.bounce = randomFloat(0.01f, 0.0f);
        particle.f = randomFloat(0.02f, 0.0f);

        particle.effectProperties = properties;
        particle.effectProperties.texture = properties.texture;
        particle.p = pos;
        particle.active = true;
    }

    properties.active = true;
}

void SparkleEffect::update(const float deltaTime) {
    bool anyActive = false;
    for (auto &s: particles) {
        if (s.active) {
            s.update(deltaTime);
            if (s.active) anyActive = true;
        }
    }
    properties.active = anyActive;
}

void SparkleEffect::draw() {
    for (const auto &s: particles) {
        if (s.active) s.draw();
    }
}

bool SparkleEffect::isActive() const {
    return properties.active;
}
