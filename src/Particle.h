// Particle.h
#pragma once
#include "EffectDefinitions.h"

class Particle {
public:
    Particle() = default;

    explicit Particle(const TextureResource &tex);

    ParticleEffectProperties effectProperties;
    bool active = true;
    GLfloat size = 0.0f;
    int life = 0;
    int lifeLeft = 0;
    position p{}, v{};
    GLfloat bounce = 0.0f, f = 0.0f;

    void update(float deltaTime);

    void draw() const;
};
