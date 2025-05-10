// game_object.h
#pragma once

#include <epoxy/gl.h>
#include "Texture.h"

class GameObject {
public:
    //     GLfloat color[3];
    GLfloat opacity; // This is still used, because it can then be reset, and it's used for fading out bricks (i think)
    GLfloat posx, posy;
    GLfloat width, height;
    GLuint dl; // opengl display list
    bool active;
    bool collide;
    bool reflect; // NOTE: use this for bricks that are not going to reflect the ball? (trap brick? :D)

    Texture tex;
};
