// moving_object.h
#pragma once

#include "game_object.h"
#include <epoxy/gl.h>

class moving_object : public game_object {
public:
    GLfloat xvel, yvel, velocity;

    moving_object() : xvel(0.0f), yvel(0.0f) {}
};
