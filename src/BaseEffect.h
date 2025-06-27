// BaseEffect.h
#pragma once

#include "EffectDefinitions.h"

class BaseEffect {
public:
    virtual ~BaseEffect() = default;

    virtual void init(const position &pos) = 0;

    virtual void update(float deltaTime) = 0;

    virtual void draw() = 0;

    [[nodiscard]] virtual bool isActive() const = 0;

    [[nodiscard]] int getEffectId() const { return properties.effectId; }

protected:
    BaseEffectProperties properties;
};
