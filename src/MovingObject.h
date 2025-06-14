// MovingObject.h
#pragma once

#include <epoxy/gl.h>
#include "GameObject.h"

class MovingObject : public GameObject {
public:
    explicit MovingObject(const texture &tex) : GameObject(tex) {
    }

    GLfloat xvel{0.0f}, yvel{0.0f}, velocity{0.0f};

    virtual void update(float deltaTime) = 0;
};
