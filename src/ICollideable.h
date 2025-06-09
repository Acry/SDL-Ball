// ICollideable.h
#pragma once

enum class CollisionType {
    None = 0,
    Ball = 1,
    Paddle = 2,
    Brick = 3,
    PowerUp = 4,
    BorderLeft = 5,
    BorderRight = 6,
    BorderTop = 7
};

class ICollideable {
public:
    virtual ~ICollideable() = default;

    [[nodiscard]] virtual float getPosX() const = 0;

    [[nodiscard]] virtual float getPosY() const = 0;

    [[nodiscard]] virtual float getWidth() const = 0;

    [[nodiscard]] virtual float getHeight() const = 0;

    [[nodiscard]] virtual bool isActive() const = 0;

    virtual void setActive(bool value) = 0;

    [[nodiscard]] virtual CollisionType getCollisionType() const = 0;
};
