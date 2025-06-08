#include "PowerupManager.h"
#include <chrono>

PowerupManager::PowerupManager(IEventManager* evtMgr, TextureManager* texMgr, const int maxPowerupsCount)
    : eventManager(evtMgr),
      textureManager(texMgr),
      maxPowerups(maxPowerupsCount),
      dropChance(0.3f) {

    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng = std::mt19937(seed);
    dropChanceDist = std::uniform_real_distribution<float>(0.0f, 1.0f);

    // Vorallokieren des Powerup-Pools
    powerups.reserve(maxPowerups);
    for (int i = 0; i < maxPowerups; i++) {
        powerups.emplace_back();
    }
}

void PowerupManager::init() {
    // Initialisiere alle Powerups
    for (auto &powerup: powerups) {
        powerup.init();
    }
}

void PowerupManager::update(const float deltaTime) {
    // Aktualisiere alle aktiven Powerups
    for (auto &powerup: powerups) {
        if (powerup.isActive()) {
            powerup.update(deltaTime);

            // Der CollisionManager prüft die Kollision und löst bei Erfolg selbst ein Event aus
            // Wir müssen nur aktualisieren und bei Kollision deaktivieren
            if (collisionManager->checkPaddlePowerupCollision(powerup)) {
                powerup.deactivate();
            }
        }
    }
}

void PowerupManager::draw(const float deltaTime) {
    // Zeichne alle aktiven Powerups
    for (auto &powerup: powerups) {
        if (powerup.isActive()) {
            powerup.draw(deltaTime);
        }
    }
}

void PowerupManager::spawnPowerup(const float posX, const float posY) {
    // Suche ein inaktives Powerup
    for (auto &powerup: powerups) {
        if (!powerup.isActive()) {
            // Zufälliger Powerup-Typ
            const auto type = static_cast<PowerupType>(powerupDist(rng));
            powerup.activate(type, posX, posY);
            return;
        }
    }
}

void PowerupManager::trySpawnPowerup(const float posX, const float posY) {
    // Würfle, ob ein Powerup erscheinen soll
    if (dropChanceDist(rng) <= dropChance) {
        spawnPowerup(posX, posY);
    }
}

void PowerupManager::onLevelLoaded(const LevelData &data) {
    powerupData = data.powerupData;
}

void PowerupManager::onBrickDestroyed(const EventData& data) {
    const float randomValue = dropChanceDist(rng);
    if (randomValue <= dropChance) {
        PowerupType type = getRandomPowerupType();
        createPowerup(data.posX, data.posY, type);
    }
}

void PowerupManager::createPowerup(float posX, float posY, PowerupType type) {
    for (auto& powerup : powerups) {
        if (!powerup.isActive()) {
            powerup.active = true;
            powerup.pos_x = posX;
            powerup.pos_y = posY;
            powerup.texture = textureManager->getPowerupTexture(type);
            return;
        }
    }
}
