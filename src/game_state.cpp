// game_state.cpp
#include "game_state.h"

#include <random>
#include <SDL2/SDL.h>

#include "config.h"

// Timing variable definitions
int globalTicks = 0;
float globalMilliTicks = 0.0f;
Uint32 nonpausingGlobalTicks = 0;
float nonpausingGlobalMilliTicks = 0.0f;
int globalTicksSinceLastDraw = 0;
float globalMilliTicksSinceLastDraw = 0.0f;

vars var;
gameVars gVar;
// current player
player_struct player;
// save game player
player_struct SOLPlayer;
difficultyStruct fixed_difficulty;
difficultyStruct runtime_difficulty;

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
    var.menuShown = 0;
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

// helper math
float random_float(const float total, const float negative) {
    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution dist(-negative, total - negative);
    return dist(rng);
}

// 2D Line Segment Intersection
int LinesCross(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, GLfloat *linx,
               GLfloat *liny) {
    float d = (x1 - x0) * (y3 - y2) - (y1 - y0) * (x3 - x2);
    if (fabs(d) < 0.001f) { return -1; }
    float AB = ((y0 - y2) * (x3 - x2) - (x0 - x2) * (y3 - y2)) / d;
    if (AB > 0.0 && AB < 1.0) {
        float CD = ((y0 - y2) * (x1 - x0) - (x0 - x2) * (y1 - y0)) / d;
        if (CD > 0.0 && CD < 1.0) {
            *linx = x0 + AB * (x1 - x0);
            *liny = y0 + AB * (y1 - y0);
            return 1;
        }
    }
    return 0;
}
