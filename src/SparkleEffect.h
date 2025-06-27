// SparkleEffect.h
#pragma once
#include "BaseEffect.h"
#include "Particle.h"

class SparkleEffect final : public BaseEffect {
public:
    SparkleEffect();

    ~SparkleEffect() override;

    void init(const position &pos) override;

    void update(float deltaTime) override;

    void draw() override;

    [[nodiscard]] bool isActive() const override;

    void setTexture(const TextureResource &tex) { properties.texture = tex; }

    void setColor(const Color &color) { properties.color = color; }

    void setSize(float s) { properties.size = s; }

    void setLife(int life) { properties.life = life; }

    void setParticleCount(int count) { properties.num = count; }

private:
    std::vector<Particle> particles;
    position spawnPos{};

protected:
    ParticleEffectProperties properties;
};
