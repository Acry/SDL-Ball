#include "PowerupManager.h"
#include <chrono>

PowerupManager::PowerupManager(EventManager *eventMgr, CollisionManager *collMgr, const int maxPowerupsCount)
    : eventManager(eventMgr),
      collisionManager(collMgr),
      maxPowerups(maxPowerupsCount),
      dropChance(0.3f) // 30% Standardchance
{
    // Initialisiere den Zufallsgenerator mit aktuellem Timestamp
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng = std::mt19937(seed);
    powerupDist = std::uniform_int_distribution<int>(0, POWERUP_COUNT - 1);
    dropChanceDist = std::uniform_real_distribution<float>(0.0f, 1.0f);

    // Erstelle den Powerup-Pool
    powerups.reserve(maxPowerups);
    for (int i = 0; i < maxPowerups; i++) {
        powerups.emplace_back(eventManager);
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

void onLevelChanged(const LevelEventData& data) {
    for (const auto& PowerUpType : data.powerupTypes) {
        // PowerUp-Informationen speichern
        // Wird erst aktiv wenn der zugehörige Brick zerstört wird
    }
}

void onBrickDestroyed(const BrickEventData& data) {
    // PowerUp mit gegebenem Typ und Position spawnen
    createPowerup(data.position, getPowerupTypeForBrick(data.id));
}

void PowerupManager::onLevelChanged(const LevelEventData& data) {
    powerupData = data.powerupData;
}