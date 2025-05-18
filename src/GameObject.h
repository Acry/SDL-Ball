// GameObject.h
#pragma once

#include <epoxy/gl.h>
#include "SpriteSheetAnimation.h"

class GameObject {
public:
    // Basisattribute für alle Spielobjekte
    GLfloat opacity{1.0f};
    GLfloat pos_x{0.0f}, pos_y{0.0f};
    GLfloat width{0.0f}, height{0.0f};
    bool active{true};
    bool collide{true};
    bool reflect{false};
    SpriteSheetAnimation texture;

    // Virtuelle Methoden - Interface-Definition
    virtual ~GameObject() = default;
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(float deltaTime = 0.0f) = 0;

    virtual bool isCollidingWith(const GameObject& other) const;

    // Hilfsfunktionen
    bool isActive() const { return active; }
    bool isVisible() const { return opacity > 0.0f; }
};
