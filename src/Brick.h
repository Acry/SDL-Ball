// Brick.h
#pragma once

#include "colors.h"
#include "ICollideable.h"
#include "MovingObject.h"

class Brick final : public MovingObject, public ICollideable {
public:
    Color customColor{}; // from level data

    explicit Brick(const texture &tex) : MovingObject(tex) {
    }

    void init() override {
    }

    void update(float deltaTime) override {
    }

    void draw() const override;

    void setActive(const bool value) override { collisionActive = value; }
    void setVisible(const bool value) { visible = value; }

    ~Brick() override = default;

    [[nodiscard]] float getPosX() const override { return pos_x; }
    [[nodiscard]] float getPosY() const override { return pos_y; }
    [[nodiscard]] float getWidth() const override { return width; }
    [[nodiscard]] float getHeight() const override { return height; }
    [[nodiscard]] bool isVisible() const { return visible; }
    [[nodiscard]] bool isActive() const override { return collisionActive; }

    [[nodiscard]] CollisionType getCollisionType() const override {
        return CollisionType::Brick;
    }

private:
    bool collisionActive{true};

    bool visible{true};
};
