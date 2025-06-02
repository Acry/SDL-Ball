// BrickManager.h
#pragma once

#include <vector>
#include "Brick.h"
#include "TextureManager.h"
#include "EventManager.h"
#include "LevelManager.h"

class BrickManager {
    std::vector<Brick> bricks;
    EventManager* eventManager;
    TextureManager* textureManager;

    // Mapping von BrickType zu BrickTexture
    static BrickTexture getTextureForType(BrickType type);

public:
    BrickManager(EventManager* evtMgr, TextureManager* texMgr);

    void setupBricks(BrickData data);
    void update(float deltaTime);
    void draw(float deltaTime);
    void clear();

    [[nodiscard]] size_t getActiveBrickCount() const;
    [[nodiscard]] const std::vector<Brick>& getBricks() const { return bricks; }
};

