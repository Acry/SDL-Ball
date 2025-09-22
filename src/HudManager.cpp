// HudPresenter.cpp
#include "HudManager.h"

HudManager::HudManager(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager)
    : eventManager(eventManager),
      textManager(textManager),
      textureManager(textureManager),
      score(*textManager),
      clock(*textManager),
      lives(textureManager, 3) {
}

void HudManager::addPoints(const int points) {
    score.addPoints(points);
}

void HudManager::update(const float deltaTime) {
    score.update(deltaTime);
    if (clockEnabled) {
        clock.update(deltaTime);
    }
}

void HudManager::draw() const {
    score.draw();
    lives.draw();
    if (clockEnabled) {
        clock.draw();
    }
}

void HudManager::resetScore() {
    score.reset();
}

void HudManager::toggleClock() {
    clockEnabled = !clockEnabled;
}
