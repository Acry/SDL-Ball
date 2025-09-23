// HudCompositor.cpp
#include "HudCompositor.h"

HudCompositor::HudCompositor(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager)
    : eventManager(eventManager),
      textManager(textManager),
      textureManager(textureManager),
      score(*textManager),
      clock(*textManager),
      lives(textureManager, 3) {
}

void HudCompositor::addPoints(const int points) {
    score.addPoints(points);
}

void HudCompositor::update(const float deltaTime) {
    score.update(deltaTime);
    if (clockEnabled) {
        clock.update(deltaTime);
    }
}

void HudCompositor::draw() const {
    score.draw();
    lives.draw();
    if (clockEnabled) {
        clock.draw();
    }
}

void HudCompositor::resetScore() {
    score.reset();
}

void HudCompositor::toggleClock() {
    clockEnabled = !clockEnabled;
}
