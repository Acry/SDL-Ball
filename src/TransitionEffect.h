// TransitionEffect.h
#pragma once
#include "BaseEffect.h"

class TransitionEffect : public BaseEffect {
public:
    TransitionEffect();

    ~TransitionEffect() override = default;

    void init(const position &pos) override;

    void update(float deltaTime) override;

    void draw() override;

    [[nodiscard]] bool isActive() const override;

    // Setter-Methoden
    void setColor(const Color &color) { properties.color = color; }
    void setLife(int life) { properties.life = life; }
    void setType(TransitionEffectType type) { properties.type = type; }

    void activate() {
        properties.active = true;
        age = 0;
        opacity = properties.startOpacity;
    }

private:
    TransitionEffectProperties properties;
    float opacity{0.0f};
    int age{0};
};
