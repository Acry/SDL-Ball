// Powerup.h
#pragma once
#include "GameObject.h"
#include "PowerupTypes.h"

class Powerup final : public GameObject {
    void drawBase() const;

public:
    bool visible{true};
    PowerupType type{PowerupType::None};

    Powerup() = default;
    void init() override {} // Leere Implementation
    void update(float deltaTime) override {} // Leere Implementation
    void draw(float deltaTime) override;
    ~Powerup() override = default;
};
