// HudPresenter.h
#pragma once

#include "Clock.h"
#include "Lives.h"
#include "Score.h"
#include "TextManager.h"
#include "TextureManager.h"

class HudManager {
    IEventManager *eventManager;
    TextManager *textManager;
    TextureManager *textureManager;
    Score score;
    Clock clock;
    Lives lives;
    bool clockEnabled = true;

public:
    HudManager(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager);

    void addPoints(int points);

    void update(float deltaTime);

    void draw() const;

    void resetScore();

    void toggleClock();
};
