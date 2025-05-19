// HUD
#include "Score.h"
#include "TextManager.h"

void Score::reset() {
    lastScoreTick = SDL_GetTicks();
    tempScore = 0;
    score = 0;
    tempText[0] = '0';
    tempText[1] = '\0';
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
    text.write(tempText, FONT_HIGHSCORE, false, 0.75f, -0.9f, 0.9f);
}
