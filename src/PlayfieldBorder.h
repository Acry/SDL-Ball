// PlayfieldBorder.h
#pragma once

#include "EventManager.h"
#include "GameObject.h"
#include "ICollideable.h"

class PlayfieldBorder final : public GameObject, public ICollideable {
public:
    enum class Side { Left, Right, Top };

    PlayfieldBorder(Side side, const texture &tex, IEventManager *eventManager);

    void setActive(bool) override {
    }

    void update(const float deltaTime);

    void init() override;

    void createDisplayList();

    void draw() const override;

    // ICollideable Implementierung
    [[nodiscard]] float getPosX() const override { return pos_x; }
    [[nodiscard]] float getPosY() const override { return pos_y; }
    [[nodiscard]] float getWidth() const override { return width; }
    [[nodiscard]] float getHeight() const override { return height; }
    [[nodiscard]] bool isActive() const override { return true; }

    [[nodiscard]] CollisionType getCollisionType() const override;

    void onCollision(const ICollideable *other, float hitX, float hitY);

private:
    Side side;
    GLuint displayList = 0;
    IEventManager *eventManager;

    void handleLevelLoaded(const LevelData &data);

    int dropSpeed{};
    float dropTimerMs = 0.0f;
};
