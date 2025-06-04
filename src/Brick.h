// Brick.h
#pragma once
#include <epoxy/gl.h>
#include "GameObject.h"
#include "ICollideable.h"

enum class BrickType {
    Base, // Typ D: Custom Color Brick
    Blue, // Typ 1: Blauer Brick
    Yellow, // Typ 2: Gelber Brick
    Cement, // Typ 3: Zement/harter Brick
    Glass, // Typ 4: Glas Brick (2 Treffer)
    Green, // Typ 5: Grüner Brick
    Grey, // Typ 6: Grauer Brick
    Purple, // Typ 7: Violetter Brick
    White, // Typ 8: Weißer Brick
    Invisible, // Typ 9: Unsichtbarer Brick (3 Treffer)
    Red, // Typ A: Roter Brick
    Explosive, // Typ B: Explosiver Brick
    Doom, // Typ C: Doom Brick (böse Powerups)
    None, // Typ 0: Kein Brick
    Count // Anzahl der Brick-Typen
};

class Brick : public GameObject, public ICollideable {
    void drawBase() const;

    bool isDestroyed{false}; // Flag to check if the brick is destroyed
    EventManager *eventManager;

public:
    Uint32 id{0}; // Unique ID for the brick
    GLfloat health{1.0f}; // Health of the brick, 1.0 means full health
    bool visible{true}; // Visibility of the brick
    BrickType type{BrickType::None};

    // Custom Color Support
    struct Color {
        float r{1.0f};
        float g{1.0f};
        float b{1.0f};
        float a{1.0f};
    } color;

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
