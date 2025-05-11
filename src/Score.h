#pragma once

#include <SDL2/SDL.h>
#include "TtfLegacyGl.h"

class Score {
    TtfLegacyGl& text;
    unsigned int lastScoreTick;
    Uint32 tempScore;
    Uint32 score;
    char tempText[255];

public:
    Score() : text(TtfLegacyGl::getInstance()),
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
