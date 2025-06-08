// Powerup.h
#pragma once
#include "MovingObject.h"
#include "SpriteSheetAnimation.h"
#include "PowerupTypes.h"

class Powerup final : public MovingObject {
    PowerupType type;
    bool active;
    EventManager *eventManager;
    SpriteSheetAnimation texture;

    static constexpr float GRAVITY = 0.7f;
    static constexpr float DEFAULT_WIDTH = 0.055f / 1.25f;
    static constexpr float DEFAULT_HEIGHT = 0.055f / 1.25f;

public:
    explicit Powerup(EventManager *eventMgr);

    ~Powerup() = default;

    void init() override;

    void update(float deltaTime) override;

    void draw(float deltaTime) override;

    void activate(PowerupType powerupType, float posX, float posY);

    void deactivate();

    [[nodiscard]] bool isActive() const { return active; }
    [[nodiscard]] PowerupType getType() const { return type; }

    [[nodiscard]] int getPoints() const;
};
