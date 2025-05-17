// Player.h
#pragma once
#include "config.h"
#include <array>
#include <SDL2/SDL_stdinc.h>

class Player {
private:
    int difficulty;
    std::array<int, MAXPOTEXTURES> powerupTimeLeft;

public:
    // Konstruktor/Destruktor
    Player();
    ~Player() = default;

    void init();
    void reset();
    void addLife();
    void removeLife();
    void addScore(int points);
    void addCoins(int amount);
    void initDifficulty(int startingDifficulty);

    // Powerup API
    bool hasPowerup(int type) const;
    void activatePowerup(int type, int duration = 0);
    void clearPowerups();
    void updatePowerups(float deltaTime);

    // Spielervariablen
    Uint32 coins;
    Uint32 multiply;
    Uint32 currentLevel;
    Uint32 lastClearedLevel;
    Uint32 lives;
    Uint32 score;
    bool explodePaddle;
    std::array<bool, MAXPOTEXTURES> powerup;
    difficultyStruct startingDifficulty;
    difficultyStruct currentDifficulty;
};
