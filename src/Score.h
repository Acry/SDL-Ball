// Score.h
#pragma once

#include <SDL2/SDL.h>
#include "TextManager.h"

class Score {
    TextManager &text;
    float timeSinceLastUpdate;
    int tempScore;
    int score;
    char tempText[255]{};

public:
    explicit Score(TextManager &textManager);

    void reset();

    void update(float deltaTime);

    void draw() const;

    void addPoints(int points);
};
