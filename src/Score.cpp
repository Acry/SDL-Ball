// HUD
#include "Score.h"
#include "text.h"

extern glTextClass *glText;

Score::Score() {
    init();
}

void Score::init() {
    tempScore = 1;
    score = 0;
    lastScoreTick = SDL_GetTicks();
}

void Score::update(const int point) {
    score = point;
}

void Score::draw() {
    const int dif = score - tempScore;

    if (tempScore != score) {
        if (lastScoreTick + 50 < SDL_GetTicks()) {
            tempScore += static_cast<float>(dif) / 7.0f + 1.0f;
            if (tempScore > score)
                tempScore = score;
            lastScoreTick = SDL_GetTicks();

            sprintf(tempText, "%i", tempScore);
        }
    }
    glText->write(tempText, FONT_HIGHSCORE, false, 0.75f, -0.9f, 0.9f);
}

