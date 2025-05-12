#pragma once
#include <epoxy/gl.h>
#include "game_state.h"
#include "MovingObject.h"
#include "Tracer.h"
#include "Paddle.h"


class Ball : public MovingObject {
    GLfloat rad;
    bool growing, shrinking;
    GLfloat destwidth, growspeed;

    static float bounceOffAngle(const GLfloat width, GLfloat posx, GLfloat hitx);

public:
    Tracer tail;
    bool explosive;
    bool glued;
    GLfloat gluedX;
    GLfloat bsin[32], bcos[32];
    bool aimdir;
    Texture fireTex;
    GLfloat lastX, lastY;

    Ball();

    void hit(GLfloat c[]);

    void move();

    void draw(const Paddle &paddle);

    GLfloat getRad();

    void setangle(GLfloat o);

    void setspeed(GLfloat v);

    void setSize(GLfloat s);

    static void checkPaddleCollision(Ball &b, const Paddle &p, position &po);
};
