// src/game_state.h
#pragma once
#include <SDL_stdinc.h>
#include <string>
#include <epoxy/gl.h>

// MENU SIZES -> TODO: Move to Menu.h
#define MENU_HALF_WIDTH 0.35f
#define MENU_WIDTH 0.85f
#define MENU_HALF_HEIGHT 0.07f
#define MENU_HEIGHT 0.14f
#define MENU_SPACING 0.18f

// "Max powerups"
#define MAXPOTEXTURES 21
#define MAXBALLS 16
#define MAXPOWERUPS 64
#define NUMITEMSFORSALE 13

#define PO_COIN 0
#define PO_BIGBALL    1
#define PO_NORMALBALL 2
#define PO_SMALLBALL  3
#define PO_EXPLOSIVE  4
#define PO_GLUE 5
#define PO_MULTIBALL 6
#define PO_GROWPADDLE 7
#define PO_SHRINKPADDLE 8
#define PO_AIM 9
#define PO_GUN 10
#define PO_THRU 11
#define PO_LASER 12
#define PO_LIFE 13
#define PO_DIE 14
#define PO_DROP 15
#define PO_DETONATE 16
#define PO_EXPLOSIVE_GROW 17
#define PO_EASYBRICK 18
#define PO_NEXTLEVEL 19
#define PO_AIMHELP 20

// Timing variables
extern int globalTicks;
extern float globalMilliTicks;
extern Uint32 nonpausingGlobalTicks;
extern float nonpausingGlobalMilliTicks;
extern int globalTicksSinceLastDraw;
extern float globalMilliTicksSinceLastDraw;

struct gameVars {
    bool shopNextItem, shopPrevItem, shopBuyItem; // When set to 1 shop goes next or prev
    int deadTime; // I hvor mange millisekunder har bolden intet rørt
    bool nextlevel;
    bool gameOver;
    bool newLife;
    bool newLevel; // Start en ny level
    int bricksleft; // hvor mange brikker er der tilbage
};

struct scrollInfoScruct {
    bool drop; // 0 right, 1 left, 2 up, 3 down
    unsigned int dropspeed;
    unsigned int lastTick; // what was the time last they moved
};

struct vars {
    bool titleScreenShow;
    int frame;
    bool paused;
    int menuShown;
    int menuItemHovered;
    bool menuPressed;
    int menuNumItems;
    int menuJoyCalStage;
    bool quit;
    bool wiiConnect;

    int numlevels;
    bool transition_half_done;
    bool clearScreen;
    bool idiotlock; // transition
    bool bricksHit; // tells the mainloop if it should copy the updated array of brick status.

    GLfloat averageBallSpeed;
    int showHighScores;
    bool enterSaveGameName;
    bool startedPlaying;

    int effectnum;

    scrollInfoScruct scrollInfo;
};

struct position {
    GLfloat x;
    GLfloat y;
};

struct difficulty {
    GLfloat ballspeed[3];
    GLfloat maxballspeed[3];
    GLfloat hitbrickinc[3];
    GLfloat hitpaddleinc[3];
    GLfloat slowdown[3];
    GLfloat speedup[3];
    Uint32 coins[3];
    Uint32 life[3];
    };

struct highscoreData {
    int points;
    std::string level;
    std::string name;
};

extern vars var;
extern gameVars gVar;

void initNewGame();

void pauseGame();

void resumeGame();

void set_fixed_difficulty();

void resetPlayerPowerups();

int LinesCross(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, GLfloat *linx,
               GLfloat *liny);
