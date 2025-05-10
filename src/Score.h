#pragma once

#include <SDL2/SDL.h>

class Score {
    unsigned int lastScoreTick;
    uint tempScore;
    uint score;
    char tempText[255];

public:
    Score();
    void init();
    void update(const int point);
    void draw();
};
