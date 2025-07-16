#pragma once

#include "Clock.h"
#include "Lifes.h"
#include "Score.h"
#include "TextManager.h"
#include "TextureManager.h"

class HudManager {
    IEventManager *eventManager;
    TextManager *textManager;
    TextureManager *textureManager;
    Score score;
    Clock clock;
    Lifes lifes;
    bool clockEnabled = true;

public:
    HudManager(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager);

    void addPoints(int points);

    void update(float deltaTime);

    void draw();

    void resetScore();

    void toggleClock();
};


