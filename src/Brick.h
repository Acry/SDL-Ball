#pragma once
#include <epoxy/gl.h>
#include "GameObject.h"
#include "ICollideable.h"
#include "EventManager.h"

class Brick : public GameObject, public ICollideable {
    void drawBase() const;

    GLfloat health{1.0f}; // Health of the brick, 1.0 means full health
    bool isDestroyed{false}; // Flag to check if the brick is destroyed
    EventManager *eventManager;

public:
    void draw(float deltaTime) override;

    explicit Brick(EventManager *eventMgr);

    void init() override;

    int getCollisionType() const override;

    void update(float deltaTime) override {
        // Bricks haben kein spezielles Update-Verhalten
    }

    float getPosX() const override { return pos_x; }
    float getPosY() const override { return pos_y; }
    float getWidth() const override { return width; }
    float getHeight() const override { return height; }
    bool isActive() const override { return GameObject::isActive(); }
    void onCollision(ICollideable *other, float hitX, float hitY) override {
        // Verhalten bei Kollision
        if (other->getCollisionType() == static_cast<int>(CollisionType::Ball)) {
            active = false; // Brick wird deaktiviert bei Kollision mit Ball
        }
    }
    ~Brick() override;
    void setPosition(const float x, const float y) {
        pos_x = x;
        pos_y = y;
    }
};
