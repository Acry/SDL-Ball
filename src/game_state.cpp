// game_state.cpp
#include "game_state.h"
#include <SDL2/SDL.h>

#include "config.h"


vars var;
gameVars gVar;
// current player
player_struct player;
// save game player
player_struct SOLPlayer;
difficultyStruct fixed_difficulty;
difficultyStruct runtime_difficulty;

void resetPlayerPowerups();

void initNewGame() {
    player.level = 0;
    player.score = 0;
    gVar.deadTime = 0;

    gVar.newLevel = true;
    gVar.newLife = true;

    player.multiply = 1;

    switch (player.difficulty) {
        case EASY:
            player.coins = 600;
            player.lives = 5;
            break;
        case NORMAL:
            player.coins = 300;
            player.lives = 3;
            break;
        case HARD:
            player.coins = 0;
            player.lives = 3;
            break;
        default: ;
    }
    resetPlayerPowerups();
}

void pauseGame() {
    var.paused = true;
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void resumeGame() {
#ifndef DEBUG_NO_RELATIVE_MOUSE
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    var.paused = false;
    var.menu = 0;
}

void set_fixed_difficulty() {
    fixed_difficulty.ballspeed[EASY] = 0.7f;
    fixed_difficulty.ballspeed[NORMAL] = 1.3f;
    fixed_difficulty.ballspeed[HARD] = 1.6f;

    fixed_difficulty.maxballspeed[EASY] = 1.5f;
    fixed_difficulty.maxballspeed[NORMAL] = 2.2f;
    fixed_difficulty.maxballspeed[HARD] = 3.0f;

    fixed_difficulty.hitbrickinc[EASY] = 0.0025;
    fixed_difficulty.hitbrickinc[NORMAL] = 0.003;
    fixed_difficulty.hitbrickinc[HARD] = 0.004;

    fixed_difficulty.hitpaddleinc[EASY] = -0.001;
    fixed_difficulty.hitpaddleinc[NORMAL] = -0.0005;
    fixed_difficulty.hitpaddleinc[HARD] = -0.0007;

    //Percentage
    fixed_difficulty.speedup[EASY] = 10.0f;
    fixed_difficulty.speedup[NORMAL] = 20.0f;
    fixed_difficulty.speedup[HARD] = 30.0f;
}

void resetPlayerPowerups() {
    for (bool &i: player.powerup) {
        i = false;
    }
}
