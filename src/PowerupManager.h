// PowerupManager.h
#pragma once
#include <vector>
#include <random>
#include "Powerup.h"
#include "IEventManager.h"
#include "TextureManager.h"

class PowerupManager {
    std::vector<Powerup> powerups;
    IEventManager *eventManager;
    TextureManager* textureManager;
    PowerupData powerupData;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dropChanceDist;

    const int maxPowerups;
    float dropChance;

    void createPowerup(float posX, float posY, PowerupType type);

    PowerupType getRandomPowerupType();

    PowerupType getPowerupTypeForBrick(size_t brickIndex);

public:
    PowerupManager(IEventManager* evtMgr, TextureManager* texMgr, int maxPowerupsCount = 5);
    ~PowerupManager() = default;

    void init();
    void update(float deltaTime);
    void draw(float deltaTime);

    void onLevelLoaded(const LevelData &data);

    void onBrickDestroyed(const EventData &data);

    void setDropChance(float chance) { dropChance = std::min(1.0f, std::max(0.0f, chance)); }
    [[nodiscard]] float getDropChance() const { return dropChance; }
};
