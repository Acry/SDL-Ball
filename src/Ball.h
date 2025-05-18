#pragma once
#include <epoxy/gl.h>
#include "game_state.h"
#include "MovingObject.h"
#include "Tracer.h"
#include "Paddle.h"
#include "GrowableObject.h"

class Ball : public MovingObject, public GrowableObject {
    GLfloat rad;
    bool growing, shrinking;
    GLfloat destwidth, growspeed;

    static float bounceOffAngle(const GLfloat width, GLfloat posx, GLfloat hitx);

protected:
    void onSizeChanged() override;

public:
    Tracer tail;
    bool explosive;
    bool glued;
    GLfloat gluedX;
    GLfloat bsin[32], bcos[32];
    bool aimdir;
    SpriteSheetAnimation fireTex;
    GLfloat lastX, lastY;

    Ball();

    // Implementation der virtuellen Getter/Setter aus GrowableObject
    GLfloat getWidth() const override { return MovingObject::width; }
    GLfloat getHeight() const override { return MovingObject::height; }
    void setWidth(GLfloat w) override { MovingObject::width = w; }
    void setHeight(GLfloat h) override { MovingObject::height = h; }

    void hit(GLfloat c[]);
    void move();
    void draw(const Paddle &paddle);
    GLfloat getRad();
    void setangle(GLfloat o);
    void setspeed(GLfloat v);
    void setSize(GLfloat s);

    static void checkPaddleCollision(Ball &b, const Paddle &p, position &po);
};
