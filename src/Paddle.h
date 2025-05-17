#pragma once
#include "GameObject.h"
#include "SpriteSheetAnimation.h"

class Paddle final : public GameObject {
    GLfloat growspeed;
    GLfloat destwidth;
    GLfloat aspect;
    bool growing;

    void drawBase();
    void drawGlueLayer() const;
    void drawGunLayer() const;
    void updateGrowth(float deltaTime);

public:
    bool dead;
    bool hasGlueLayer;
    bool hasGunLayer;
    SpriteSheetAnimation *layerTex;

    Paddle();
    void init() override;
    void update(float deltaTime) override;
    void moveTo(float targetX, float deltaTime);
    void draw() override;
    void grow(GLfloat width);

    // Powerup-Setter
    void setGlueLayer(bool enabled);
    void setGunLayer(bool enabled);
};
