#pragma once

#include "Paddle.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"
#include "EventDispatcher.h"
#include <vector>

class PaddleManager {
    IEventManager *eventManager;
    TextureManager *textureManager;
    SpriteSheetAnimationManager *animationManager;
    std::vector<size_t> animationIndices;
    float moveTargetX = 0.0f;

public:
    Paddle *activePaddle = nullptr;
    float paddleSpeed = 10.0f;

    PaddleManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr);

    ~PaddleManager();

    Paddle *getActivePaddle() const;

    void setGlueLayer(bool enabled) const;

    void setGunLayer(bool enabled) const;

    void despawn();

    void spawn();

    void update(float deltaTime) const;

    void draw() const;

    void clear();

private:
    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data);

    void handleKeyboardMove(const KeyboardMoveEventData &data);

    void moveTowardsTarget(float deltaTime) const;

    void checkBorderCollision() const;

    void checkPaddleToPowerupCollision() const;
};
