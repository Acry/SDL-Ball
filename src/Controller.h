#pragma once

#include <SDL2/SDL.h>
#include <epoxy/gl.h>
#include "Paddle.h"
#include "BallManager.h"
#include "Bullets.h"

class Controller {
    paddle_class* paddle;
    bulletsClass* bullet;
    BallManager* bMan;

    int shotTime;
    int itemSelectTime;
    float accel;

    SDL_Joystick* joystick;
    Sint16 joystickx;
    Sint16 joysticky;
    Uint8 joystickbtnA;
    Uint8 joystickbtnB;
    bool joyBtnALock;

    GLfloat joystickLeftX;
    GLfloat joystickRightX;
    int calMin;
    int calMax;
    int calLowJitter;
    int calHighJitter;

    void movePaddle(GLfloat px);
    void btnPress();

public:
    Controller(paddle_class* pc, bulletsClass* bu, BallManager* bm);
    ~Controller();

    bool get();
    void calibrate();
    bool joystickAttached() const;
};
