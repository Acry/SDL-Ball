extern SettingsManager settingsManager;

class controllerClass {
    paddle_class *paddle;
    bulletsClass *bullet;
    BallManager *bMan;
#define ITEMSELECTTIME 300 /* Milliseconds before changing item */
    int shotTime, itemSelectTime;
    float accel;
    SDL_Joystick *joystick;
    Sint16 joystickx;
    Sint16 joysticky;
    Uint8 joystickbtnA, joystickbtnB;
    bool joyBtnALock; //TODO: implement this in the rest of the joystick functions (current only using it in cal)
    GLfloat joystickLeftX; //There are two because the calibrated values differ
    GLfloat joystickRightX;
    int calMin, calMax, calLowJitter, calHighJitter;

public:
    controllerClass(paddle_class *pc, bulletsClass *bu, BallManager *bm);

    ~controllerClass();

    void movePaddle(GLfloat px);

    void btnPress();

    bool get();

    void calibrate();

    bool joystickAttached() const;
};

controllerClass::controllerClass(paddle_class *pc, bulletsClass *bu, BallManager *bm) {
    paddle = pc;
    bullet = bu;
    bMan = bm;
    shotTime = 200;
    itemSelectTime = 0;

    //Try to open a joystick.
    if (setting.joyEnabled && SDL_NumJoysticks() > 0) {
        joystickLeftX = setting.controlMaxSpeed / setting.JoyCalMin;
        joystickRightX = setting.controlMaxSpeed / setting.JoyCalMax;
        joystick = SDL_JoystickOpen(0);
        if (joystick) {
            SDL_Log("Using joystick: '%s' as %s.", SDL_JoystickName(0),
                    (setting.joyIsDigital ? "digital" : (setting.joyIsPaddle) ? "paddle" : "analog"));
            SDL_JoystickEventState(SDL_ENABLE);
        } else {
            SDL_Log("Failed to open joystick: '%s'", SDL_JoystickName(0));
        }
    }
}

controllerClass::~controllerClass() {
    SDL_JoystickClose(0);
}

void controllerClass::movePaddle(GLfloat px) {
    if (!var.paused) {
        paddle->posx = px;
        if (paddle->posx > 1.66 - paddle->width - 0.06) {
            paddle->posx = 1.66 - paddle->width - 0.06;
        } else if (paddle->posx < -1.66 + paddle->width + 0.06) {
            paddle->posx = -1.66 + paddle->width + 0.06;
        }
    }
}

void controllerClass::btnPress() {
    if (var.titleScreenShow) {
        var.titleScreenShow = 0;
        SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, 0);
        return;
    }

    pos p;
    if (shotTime > 150) {
        shotTime = 0;
        if (!var.menu && !var.paused) {
            bMan->unglue();
            var.startedPlaying = 1;
            if (player.powerup[PO_GUN]) {
                p.x = paddle->posx - paddle->width / 1.5;
                p.y = paddle->posy;
                bullet->shoot(p);
                p.x = paddle->posx + paddle->width / 1.5;
                bullet->shoot(p);
            }
        }
    }
}

bool controllerClass::get() {
    const Uint8 *keyStates;
    Uint8 keyDown[3]; //Need this since its not a good idea to write to keyStates for some reason
    shotTime += globalTicks;
    SDL_PumpEvents();
    keyStates = SDL_GetKeyboardState(NULL);
    keyDown[0] = keyStates[setting.keyLeft];
    keyDown[1] = keyStates[setting.keyRight];
    keyDown[2] = keyStates[setting.keyShoot];

    itemSelectTime += globalTicks;
    //Read joystick here so we can override keypresses if the joystick is digital
    //We shouldn't need to check if the joystick is enabled, since it won't be opened if its not enabled anyway.
    if (setting.joyEnabled && joystick) {
        joystickx = SDL_JoystickGetAxis(joystick, 0);
        joysticky = SDL_JoystickGetAxis(joystick, 1);
        joystickbtnA = SDL_JoystickGetButton(joystick, 0);
        joystickbtnB = SDL_JoystickGetButton(joystick, 1);


        if (joystickbtnA) {
            keyDown[2] = 1;
        }
        if (joystickbtnB && itemSelectTime > ITEMSELECTTIME) {
            itemSelectTime = 0;
            gVar.shopBuyItem = 1;
        }

        if (setting.joyIsDigital) {
            if (joystickx < -200) {
                keyDown[0] = 1;
            } else if (joystickx > 200) {
                keyDown[1] = 1;
            }
            if (joysticky < -200 && itemSelectTime > ITEMSELECTTIME) {
                itemSelectTime = 0;
                gVar.shopNextItem = 1;
            } else if (joysticky > 200 && itemSelectTime > ITEMSELECTTIME) {
                itemSelectTime = 0;
                gVar.shopPrevItem = 1;
            }
        } else if (setting.joyIsPaddle) {
            GLfloat abx = (166 / (32767.0 / 2.0)) * (((float) joystickx) - (32767.0 / 2.0));
            movePaddle(abx / 100.0);
        } else {
            GLfloat x = 0.0; //This is the actual traveling speed of the paddle
            if (joystickx > setting.JoyCalHighJitter) {
                x = joystickRightX * joystickx;
            } else if (joystickx < setting.JoyCalLowJitter) {
                x = -(joystickLeftX * joystickx);
            }

            if (joysticky < setting.JoyCalLowJitter && itemSelectTime > ITEMSELECTTIME) {
                itemSelectTime = 0;
                gVar.shopNextItem = 1;
            } else if (joysticky > setting.JoyCalHighJitter && itemSelectTime > ITEMSELECTTIME) {
                itemSelectTime = 0;
                gVar.shopPrevItem = 1;
            }
            //Move the paddle:
            movePaddle(paddle->posx += (x * globalMilliTicks));
        }
    }
    //React to keystates here, this way, if joyisdig it will press keys
    if (keyDown[0]) {
        accel += globalMilliTicks * setting.controlAccel;
        if (accel > setting.controlMaxSpeed)
            accel = setting.controlMaxSpeed;
        movePaddle(paddle->posx - (accel * globalMilliTicks));
    } else if (keyDown[1]) {
        accel += globalMilliTicks * setting.controlAccel;
        if (accel > setting.controlMaxSpeed)
            accel = setting.controlMaxSpeed;
        movePaddle(paddle->posx + (accel * globalMilliTicks));
    } else {
        accel = setting.controlStartSpeed;
    }

    if (keyDown[2]) {
        btnPress();
        return true;
    }
    return false;
}

void controllerClass::calibrate() {
    Sint16 x = 0;
    if (SDL_TRUE == joystickAttached()) {
        x = SDL_JoystickGetAxis(joystick, 0);

        if (SDL_JoystickGetButton(joystick, 0)) {
            if (!joyBtnALock && var.menuJoyCalStage != 5) {
                var.menuJoyCalStage++;
                joyBtnALock = true;
            }
        } else {
            joyBtnALock = false;
        }
    }

    switch (var.menuJoyCalStage) {
        case 0:
            calMin = 0;
            calMax = 0;
            calLowJitter = 0;
            calHighJitter = 0;
            break;
        case 1:
            if (x < calLowJitter)
                calLowJitter = x;
            else if (x > calHighJitter)
                calHighJitter = x;
            break;
        case 2:
            if (x < calMin)
                calMin = x;
            break;
        case 3:
            if (x > calMax)
                calMax = x;
            break;
        case 4:
            setting.JoyCalMin = calMin;
            setting.JoyCalMax = calMax;
            setting.JoyCalLowJitter = calLowJitter;
            setting.JoyCalHighJitter = calHighJitter;
            SDL_Log("Joystick calibration report:");
            SDL_Log("calMin:%d calMax:%d", calMin, calMax);
            SDL_Log("lowJit:%d higJit:%d", calLowJitter, calHighJitter);
            var.menuJoyCalStage++;
            settingsManager.settingsChanged();
            break;
        default: ;
    }
}

bool controllerClass::joystickAttached() const {
    return joystick != nullptr;
}
