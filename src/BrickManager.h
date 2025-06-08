// BrickManager.h
#pragma once

#include <vector>
#include "Brick.h"
#include "TextureManager.h"
#include "IEventManager.h"

class BrickManager {
    std::vector<Brick> bricks;
    EventManager* eventManager;
    TextureManager* textureManager;
    std::unordered_map<size_t, float> brickHealth;
    std::unordered_map<size_t, BrickType> brickTypes;

    static BrickTexture getTextureForType(BrickType type);

public:
    BrickManager(EventManager* evtMgr, TextureManager* texMgr);

    void onCollision(size_t brickIndex, ICollideable *other, float hitX, float hitY) {
        if (other->getCollisionType() == static_cast<int>(CollisionType::Ball)) {
            EventData data;
            data.target = &bricks[brickIndex];
            onBrickHit(data);
        }
    }

    // Kollisionshilfs-Methoden
    [[nodiscard]] float getPosX(size_t index) const { return bricks[index].pos_x; }
    [[nodiscard]] float getPosY(size_t index) const { return bricks[index].pos_y; }
    [[nodiscard]] float getWidth(size_t index) const { return bricks[index].width; }
    [[nodiscard]] float getHeight(size_t index) const { return bricks[index].height; }
    [[nodiscard]] bool isActive(size_t index) const { return bricks[index].active; }
    void setupBricks(std::vector<BrickInfo> data);

    void onLevelLoaded(const LevelData &data);

    void update(float deltaTime);
    void draw(float deltaTime);

    void onBrickHit(const EventData &data);

    void onBrickDestroyed(const EventData &data);

    void clear();

    [[nodiscard]] size_t getActiveBrickCount() const;
    [[nodiscard]] const std::vector<Brick>& getBricks() const { return bricks; }
};
