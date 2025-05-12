#include "Player.h"
#include <algorithm>

#include "SettingsManager.h"

Player::Player() :
    lives(3),
    score(0),
    coins(0),
    multiply(1),
    difficulty(DEFAULT_DIFFICULTY) {
    clearPowerups();
}

void Player::init() {
    // Hole den Schwierigkeitsgrad aus den Settings
    const auto& settings = SettingsManager::getInstance(configFileManager);
    initDifficulty(settings.getDifficulty());
}

void Player::reset() {
    initDifficulty

    // Abhängig vom SaveGameManager
    currentLevel = 0;
    lastClearedLevel = 0;

    score = 0;

    multiply = 1;
    clearPowerups();
}


void Player::initDifficulty(int startingDifficulty) {
        difficulty = startingDifficulty;

        // Verwende die vordefinierten Werte aus der difficulty-Struktur
        coins = fixed_difficulty.coins[difficulty];
        lives = fixed_difficulty.life[difficulty];
}

void Player::addLife() {
    lives++;
}

void Player::removeLife() {
    lives--;
}

void Player::addScore(int points) {
    score += points * multiply;
}

void Player::addCoins(int amount) {
    coins += amount;
}

void Player::activatePowerup(int type, int duration) {
    powerup[type] = true;
    powerupTimeLeft[type] = duration;
}

void Player::clearPowerups() {
    std::fill(std::begin(powerup), std::end(powerup), false);
    std::fill(std::begin(powerupTimeLeft), std::end(powerupTimeLeft), 0);
}

void Player::updatePowerups(float deltaTime) {
    for(size_t i = 0; i < powerupTimeLeft.size(); i++) {
        if(powerup[i] && powerupTimeLeft[i] > 0) {
            powerupTimeLeft[i] -= deltaTime;
            if(powerupTimeLeft[i] <= 0) {
                powerup[i] = false;
            }
        }
    }
}
