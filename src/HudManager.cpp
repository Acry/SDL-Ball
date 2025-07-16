#include "HudManager.h"


HudManager::HudManager(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager)
    : eventManager(eventManager),
      textManager(textManager),
      textureManager(textureManager),
      score(*textManager),
      clock(*textManager),
      lifes(textureManager, 21) {
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
    lifes.draw();
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
