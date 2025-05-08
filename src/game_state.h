// src/game_state.h
#pragma once

#define MAXPOTEXTURES 21

struct gameVars {
    bool shopNextItem, shopPrevItem, shopBuyItem; //When set to 1 shop goes next or prev
    int deadTime; //I hvor mange millisekunder har bolden intet rørt
    bool nextlevel;
    bool gameOver;
    bool newLife;
    bool newLevel; //Start en ny level
    int bricksleft; //hvor mange brikker er der tilbage
};

struct player_struct {
    int coins;
    int multiply;
    bool powerup[MAXPOTEXTURES];
    bool explodePaddle; //This lock makes the paddle explode, and it won't come back until new life.
    int level;
    int lives;
    int difficulty;
    int score;
};