// game_object.h
#pragma once

#include <epoxy/gl.h>
#include "Texture.h"

class GameObject {
public:
    GLfloat opacity; // This is still used, because it can then be reset, and it's used for fading out bricks (i think)
    GLfloat pos_x, pos_y;
    GLfloat width, height;
    GLuint displayList;
    bool active;
    bool collide;
    bool reflect;
    Texture texture;
};
