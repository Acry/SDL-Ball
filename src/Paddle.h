#pragma once
#include "MovingObject.h"
#include "GrowableObject.h"
#include "SpriteSheetAnimation.h"
#include <epoxy/gl.h>

class Paddle final : public MovingObject, public GrowableObject {
    void drawBase();

    void drawGlueLayer() const;

    void drawGunLayer() const;

protected:
    void onSizeChanged() override {
    }

public:
    bool dead;
    bool hasGlueLayer;
    bool hasGunLayer;
    SpriteSheetAnimation *layerTex;

    Paddle();

    void init() override;

    void update(float deltaTime) override;

    void moveTo(float targetX, float deltaTime);

    void draw(float deltaTime); // Spezifische Methode mit deltaTime
    void grow(GLfloat targetWidth);

    // Implementation der virtuellen Getter/Setter aus GrowableObject
    [[nodiscard]] GLfloat getWidth() const override { return GameObject::width; }
    [[nodiscard]] GLfloat getHeight() const override { return GameObject::height; }
    void setWidth(const GLfloat w) override { GameObject::width = w; }
    void setHeight(const GLfloat h) override { GameObject::height = h; }

    // Powerup-Setter
    void setGlueLayer(bool enabled);

    void setGunLayer(bool enabled);
};
