#pragma once
#include "GameObject.h"
#include "GrowableObject.h"
#include "SpriteSheetAnimation.h"

class Paddle final : public GameObject, public GrowableObject {
    GLfloat aspect;

    void drawBase();
    void drawGlueLayer() const;
    void drawGunLayer() const;

protected:
    void onSizeChanged() override {}

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
    void grow(GLfloat targetWidth);

    // Implementation der virtuellen Getter/Setter aus GrowableObject
    GLfloat getWidth() const override { return GameObject::width; }
    GLfloat getHeight() const override { return GameObject::height; }
    void setWidth(GLfloat w) override { GameObject::width = w; }
    void setHeight(GLfloat h) override { GameObject::height = h; }

    // Powerup-Setter
    void setGlueLayer(bool enabled);
    void setGunLayer(bool enabled);
};
