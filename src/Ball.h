#pragma once
#include <epoxy/gl.h>
#include <vector>

#include "EventManager.h"
#include "MovingObject.h"
#include "GrowableObject.h"
#include "ICollideable.h"

class Ball final : public MovingObject, public GrowableObject, public ICollideable {
    GLfloat rad;
    EventManager *eventManager;

    mutable std::vector<float> collisionPoints;

protected:
    void onSizeChanged() override;

public:
    float centerX{0.0f};
    float centerY{0.0f};
    bool explosive{false};
    bool glued{false};
    GLfloat bsin[32]{}, bcos[32]{};
    bool aimdir{false};
    SpriteSheetAnimation fireTex;

    void launchFromPaddle();

    explicit Ball(EventManager *eventMgr);

    ~Ball() override;

    void init() override;

    // Implementation der virtuellen Getter/Setter aus GrowableObject
    [[nodiscard]] GLfloat getWidth() const override { return MovingObject::width; }
    [[nodiscard]] GLfloat getHeight() const override { return MovingObject::height; }
    void setWidth(GLfloat w) override { MovingObject::width = w; }
    void setHeight(GLfloat h) override { MovingObject::height = h; }

    void hit(GLfloat c[]);

    void update(float deltaTime) override;

    void draw(float deltaTime) override;

    GLfloat getAngle();

    void setAngle(GLfloat o);

    void setSpeed(GLfloat v, GLfloat maxSpeed = 1.5f);

    void setSize(GLfloat s);

    // ICollideable Interface
    float getPosX() const override { return pos_x; }
    float getPosY() const override { return pos_y; }
    bool isActive() const override { return GameObject::isActive(); }

    const std::vector<float> *getCollisionPoints() const override;

    void onCollision(ICollideable *other, float hitX, float hitY) override;

    int getCollisionType() const override;
};
