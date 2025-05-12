#pragma once
#include "config.h"
#include <array>
#include <SDL2/SDL_stdinc.h>

#include "game_state.h"

class Player {
    std::array<int, 40> powerupTimeLeft;
public:
    // Konstruktor/Destruktor
    Player();
    ~Player() = default;

    void init();
    // Spielzustand
    void reset();
    void addLife();
    void removeLife();
    void addScore(int points);
    void addCoins(int amount);
    void initDifficulty(int startingDifficulty);

    // Powerup API
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
    // das ist ja wohl beim Tod so und sollte nicht im Spieler sein.
    bool explodePaddle{false};
    // Was auch immer
    bool powerup[MAXPOTEXTURES];
    difficultyStruct startingDifficulty;
    difficultyStruct currentDifficulty;

    // Powerup-Array für schnellen Zugriff
    // bool powerup[40];
    };
