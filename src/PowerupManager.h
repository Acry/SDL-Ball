#pragma once
#include <vector>
#include <random>

#include "Powerup.h"
#include "EventManager.h"
#include "CollisionManager.h"

class PowerupManager {
    std::vector<Powerup> powerups;
    EventManager *eventManager;
    CollisionManager *collisionManager;

    std::mt19937 rng;
    std::uniform_int_distribution<int> powerupDist;
    std::uniform_real_distribution<float> dropChanceDist;

    int maxPowerups;
    float dropChance;

public:
    PowerupManager(EventManager *eventMgr, CollisionManager *collMgr, int maxPowerupsCount = 5);

    ~PowerupManager() = default;

    void init();

    void update(float deltaTime);

    void draw(float deltaTime);

    void spawnPowerup(float posX, float posY);

    void trySpawnPowerup(float posX, float posY);

    void setDropChance(float chance) { dropChance = std::min(1.0f, std::max(0.0f, chance)); }
    [[nodiscard]] float getDropChance() const { return dropChance; }
};
