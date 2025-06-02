// PlayfieldBorder.h
#pragma once

#include "GameObject.h"
#include "SpriteSheetAnimation.h"
#include "ICollideable.h"

class PlayfieldBorder final : public GameObject, public ICollideable {

public:
    enum class Side { Left, Right, Top };

    PlayfieldBorder(Side side, EventManager *eventManager);

    void init() override;

    void draw(float deltaTime) override;

    void update(float) override {
    }

    // ICollideable Implementierung
    float getPosX() const override { return pos_x; }
    float getPosY() const override { return pos_y; }
    float getWidth() const override { return width; }
    float getHeight() const override { return height; }

    bool isActive() const override { return GameObject::isActive(); }

    int getCollisionType() const override;

    void onCollision(ICollideable *other, float hitX, float hitY) override;
    void createDisplayList();
private:
    Side side;
    GLuint dl = 0;
};
