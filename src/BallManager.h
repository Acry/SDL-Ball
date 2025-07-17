#pragma once

#include <vector>
#include "Ball.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"
#include "EventDispatcher.h"

class BallManager {
    IEventManager *eventManager;
    TextureManager *textureManager;
    SpriteSheetAnimationManager *animationManager;
    std::vector<size_t> animationIndices;
    static constexpr int MAX_BALLS = 16;

public:
    Ball *selectedBall{nullptr};
    std::vector<Ball *> managedObjects;

    BallManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr);

    ~BallManager();

    void setExplosive(Ball *ball, bool explosive);

    void despawnBall(Ball *ball);

    void spawn(float x, float y, bool glued = false, float speed = 0.3f, float angle = 0.0f);

    void update(float deltaTime);

    void draw() const;

    void clear();

private:
    void launchBall() const;

    void checkBorderCollision() const;

    void checkBallToBallCollision() const;

    static float getRandomLaunchAngle();
};
