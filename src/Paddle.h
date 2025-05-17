#pragma once
#include "GameObject.h"
#include "SpriteSheetAnimation.h"

class Paddle : public GameObject {
    GLfloat growspeed;
    GLfloat destwidth;
    GLfloat aspect;
    bool growing;

    void drawBase();
    void drawGlueLayer();
    void drawGunLayer();
    void updateGrowth();

public:
    bool dead;
    SpriteSheetAnimation *layerTex;

    Paddle();
    void init();
    void grow(GLfloat width);
    void draw();
};
