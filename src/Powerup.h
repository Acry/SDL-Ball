// Powerup.h
#pragma once

#include "ICollideable.h"
#include "MovingObject.h"
#include "PowerupTypes.h"

class Powerup final : public MovingObject, public ICollideable {
public:
    explicit Powerup(const texture &tex) : MovingObject(tex) {
    }

    void init() override {
    }

    void update(float deltaTime) override {
    }

    void draw() const override;

    void setActive(const bool value) override { collisionActive = value; }

    ~Powerup() override = default;

    [[nodiscard]] float getPosX() const override { return pos_x; }
    [[nodiscard]] float getPosY() const override { return pos_y; }
    [[nodiscard]] float getWidth() const override { return width; }
    [[nodiscard]] float getHeight() const override { return height; }
    [[nodiscard]] bool isVisible() const { return visible; }
    [[nodiscard]] bool isActive() const override { return collisionActive; }

    [[nodiscard]] CollisionType getCollisionType() const override {
        return CollisionType::PowerUp;
    }

private:
    bool collisionActive{true};

    bool visible{true};

    PowerupType type{PowerupType::None};
};
