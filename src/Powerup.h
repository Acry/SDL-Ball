#pragma once


#include "MovingObject.h"
#include "SpriteSheetAnimation.h"
#include "EventManager.h"

enum PowerupType {
    POWERUP_GROW,
    POWERUP_SHRINK,
    POWERUP_SPEED,
    POWERUP_SLOW,
    POWERUP_MULTI,
    POWERUP_EXTRA_LIFE,
    POWERUP_EXPLOSIVE,
    POWERUP_COUNT
};

class Powerup final : public MovingObject {
    PowerupType type;
    bool active;
    EventManager *eventManager;
    SpriteSheetAnimation texture;

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
};
