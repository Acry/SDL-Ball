// ParticleEffect.h
#pragma once
#include "BaseEffect.h"
#include "Particle.h"

class ParticleEffect : public BaseEffect {
public:
    ParticleEffect() = default;

    explicit ParticleEffect(const ParticleEffectProperties &preset) {
        properties = preset;
    }

    ~ParticleEffect() override = default;

    void init(const position &pos) override;

    void update(float deltaTime) override;

    void draw() override;

    [[nodiscard]] bool isActive() const override;

    // Setter-Methoden
    void setTexture(const TextureResource &tex) { properties.texture = tex; }
    void setColor(const Color &color) { properties.color = color; }
    void setSize(float s) { properties.size = s; }
    void setLife(int life) { properties.life = life; }
    void setParticleCount(int count) { properties.count = count; }
    void setGravity(float gravity) { properties.gravity = gravity; }
    void setSpeed(float speed) { properties.speed = speed; }
    void setSpread(float spread) { properties.spread = spread; }
    void setDirection(float angle) { properties.directionAngle = angle; }

    // Multi-Color-Unterstützung
    void setColorPalette(const std::vector<Color> &colors) {
        properties.colorPalette = colors;
    }

    void enableColorPalette(bool enable) {
        properties.useColorPalette = enable;
    }

protected:
    ParticleEffectProperties properties;
    std::vector<Particle> particles;
    position spawnPos{};
};
