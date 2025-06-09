// GameObject.h
#pragma once

#include <epoxy/gl.h>
#include "SpriteSheetAnimation.h"

class EventManager;

class GameObject {
protected:
    EventManager *eventManager{nullptr};

public:
    explicit GameObject(EventManager *eventMgr = nullptr) : eventManager(eventMgr) {
    }

    GLfloat pos_x{0.0f}, pos_y{0.0f};
    GLfloat width{0.0f}, height{0.0f};

    SpriteSheetAnimation *texture{nullptr};

    virtual ~GameObject() = default;

    virtual void init() = 0;

    // SpriteSheetAnimation
    virtual void update(float deltaTime) = 0;

    virtual void draw(float deltaTime) = 0;
};
