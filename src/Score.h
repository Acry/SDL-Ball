#pragma once

#include <SDL2/SDL.h>
#include "Text.h"

class Score {
    Text& text;
    unsigned int lastScoreTick;
    uint tempScore;
    uint score;
    char tempText[255];

public:
    Score() : text(Text::getInstance()),
              lastScoreTick(SDL_GetTicks()),
              tempScore(0),
              score(0) {
        tempText[0] = '0';  // Initial-Score
        tempText[1] = '\0';
    }
    void reset();  // statt init()
    void update(const int point);
    void draw();
};
