// moving_object.h
#pragma once

#include "GameObject.h"
#include <epoxy/gl.h>

class MovingObject : public GameObject {
public:
    GLfloat xvel, yvel, velocity{};

    MovingObject() : xvel(0.0f), yvel(0.0f) {}
};
