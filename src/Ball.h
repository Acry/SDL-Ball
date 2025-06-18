#pragma once
#include <epoxy/gl.h>
#include <vector>

#include "MovingObject.h"
#include "GrowableObject.h"
#include "ICollideable.h"

class Ball final : public GrowableObject, public ICollideable {
protected:
    void onSizeChanged() override;

public:
    float centerX{0.0f};
    float centerY{0.0f};
    bool explosive{false};
    bool glued{false};
    GLfloat bsin[32]{}, bcos[32]{};
    bool aimdir{false};

    void launchFromPaddle();

    explicit Ball(const texture &tex) : GrowableObject(tex) {
    }

    void init() override;

    // void hit(GLfloat c[]);

    void update(float deltaTime) override;

    void draw() const override;

    GLfloat getAngle();

    void setAngle(GLfloat o);

    void setSpeed(GLfloat v, GLfloat maxSpeed = 1.5f);

    void setSize(GLfloat s);

    // const std::vector<float> *getCollisionPoints() const override;

    // void onCollision(const ICollideable *other, float hitX, float hitY) override;

    ~Ball() override;

    // [[nodiscard]] float getPosX() const override { return pos_x; }
    // [[nodiscard]] float getPosY() const override { return pos_y; }
    // [[nodiscard]] float getWidth() const override { return width; }
    // [[nodiscard]] float getHeight() const override { return height; }
    // [[nodiscard]] bool isVisible() const { return visible; }
    // [[nodiscard]] bool isActive() const override { return collisionActive; }

    [[nodiscard]] CollisionType getCollisionType() const override {
        return CollisionType::Ball;
    }

private:
    void drawBase() const;

    void drawExplosiveLayer() const;

    GLfloat rad{};

    mutable std::vector<float> collisionPoints;
};
