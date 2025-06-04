// PowerupManager.h
#pragma once
#include <vector>
#include <random>
#include <map>
#include "Powerup.h"
#include "EventManager.h"


class PowerupManager {
    std::vector<Powerup> powerups;
    EventManager *eventManager;
    PowerupData powerupData;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dropChanceDist;

    const int maxPowerups;
    float dropChance;

    void createPowerup(const glm::vec2 &position, PowerUpType type);

    PowerUpType getRandomPowerupType();

    PowerUpType getPowerupTypeForBrick(Uint32 brickId);

public:
    PowerupManager(EventManager *eventMgr, CollisionManager *collMgr, int maxPowerupsCount = 5);

    ~PowerupManager() = default;

    void init();
    void update(float deltaTime);
    void draw(float deltaTime);

    void onLevelChanged(const LevelEventData &data);

    void onBrickDestroyed(const EventData &data);

    void setDropChance(float chance) { dropChance = std::min(1.0f, std::max(0.0f, chance)); }
    [[nodiscard]] float getDropChance() const { return dropChance; }
};