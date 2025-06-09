// BrickManager.h
#pragma once

#include <vector>
#include "Brick.h"
#include "TextureManager.h"
#include "IEventManager.h"

class BrickManager {
    std::vector<Brick> bricks;
    IEventManager *eventManager;
    TextureManager *textureManager;
    std::unordered_map<size_t, float> brickHealth;
    std::unordered_map<size_t, BrickType> brickTypes;

    static BrickTexture getTextureForType(BrickType type);

public:
    BrickManager(IEventManager *evtMgr, TextureManager *texMgr);

    ~BrickManager();

    void setupBricks(const std::vector<BrickInfo> &data);

    void onLevelLoaded(const LevelData &data);

    void update(float deltaTime);

    void draw(float deltaTime);

    void onBallHitBrick(const CollisionData &data);

    void clear();

    [[nodiscard]] size_t getActiveBrickCount() const;

    [[nodiscard]] const std::vector<Brick> &getBricks() const { return bricks; }
};
