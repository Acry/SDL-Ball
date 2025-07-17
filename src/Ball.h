#pragma once

#include "GrowableObject.h"
#include "ICollideable.h"
#include <vector>
#include <cmath>

class Ball final : public GrowableObject, public ICollideable {
public:
    explicit Ball(const texture &tex);

    float centerX{0.0f};
    float centerY{0.0f};
    mutable std::vector<float> collisionPoints;

    void init() override;

    void draw() const override;

    void update(float deltaTime) override;

    // ICollideable Interface
    void setActive(bool value) override;

    [[nodiscard]] float getPosX() const override;

    [[nodiscard]] float getPosY() const override;

    [[nodiscard]] float getWidth() const override;

    [[nodiscard]] float getHeight() const override;

    [[nodiscard]] bool isActive() const override;

    [[nodiscard]] CollisionType getCollisionType() const override;

    // Ball-spezifische Methoden
    [[nodiscard]] float getAngle() const;

    void setAngle(float o);

    void setSpeed(float v);

    void setSize(float s);

    std::vector<float> *getCollisionPoints() const;

    void onSizeChanged() override;

    [[nodiscard]] bool isPhysicallyActive() const;

    void setPhysicallyActive(bool value);

    [[nodiscard]] bool isGlued() const;

    void setGlued(bool value);

    [[nodiscard]] bool isExplosive() const;

    void setExplosive(bool value);

private:
    bool collisionActive{true};
    bool active{false};
    bool glued{false};
    bool explosive{false};
    float angleRadians{0.0f};
};
