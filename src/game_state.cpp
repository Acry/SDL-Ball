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

vars var;
gameVars gVar;

void initNewGame() {
    player.reset();
    gVar.deadTime = 0;
    gVar.newLevel = true;
    gVar.newLife = true;
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
