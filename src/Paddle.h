#pragma once
#include "MovingObject.h"
#include "GrowableObject.h"
#include "ICollideable.h"
#include "SpriteSheetAnimation.h"
#include <epoxy/gl.h>
#include "EventManager.h"

class Paddle final : public MovingObject, public GrowableObject, public ICollideable {
    void drawBase() const;

    void drawGlueLayer() const;

    void drawGunLayer() const;

    mutable std::vector<float> collisionPoints;

protected:
    void onSizeChanged() override;

public:
    bool hasGlueLayer{};
    bool hasGunLayer{};
    SpriteSheetAnimation *layerTex{};
    float centerX{0.0f};

    explicit Paddle(EventManager *eventMgr);

    void init() override;

    void update(float deltaTime) override;

    void moveTo(float targetX, float deltaTime);

    void draw(float deltaTime) override;

    // Implementation der virtuellen Getter/Setter aus GrowableObject
    [[nodiscard]] GLfloat getWidth() const override { return GameObject::width; }
    [[nodiscard]] GLfloat getHeight() const override { return GameObject::height; }
    void setWidth(const GLfloat w) override { GameObject::width = w; }
    void setHeight(const GLfloat h) override { GameObject::height = h; }

    // Powerup-Setter
    void setGlueLayer(bool enabled);

    void setGunLayer(bool enabled);

    // ICollideable Interface-Implementierung
    float getPosX() const override { return pos_x; }
    float getPosY() const override { return pos_y; }
    bool isActive() const override { return GameObject::isActive(); }

    const std::vector<float> *getCollisionPoints() const override;

    void onCollision(ICollideable *other, float hitX, float hitY) override;

    [[nodiscard]] int getCollisionType() const override;

    ~Paddle();
};
