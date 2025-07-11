#include "Clock.h"
#include <ctime>

namespace {
    constexpr float CLOCK_UPDATE_INTERVAL = 1.0f; // seconds
}

Clock::Clock(TextManager &textManager)
    : text(textManager), timeAccumulator(0.0f) {
    clockString[0] = '\0';
}

void Clock::update(const float deltaTime) {
    timeAccumulator += deltaTime;
    if (timeAccumulator >= CLOCK_UPDATE_INTERVAL) {
        timeAccumulator = 0.0f;
        const time_t now = time(nullptr);
        const tm *local = localtime(&now);
        sprintf(clockString, "%02i:%02i", local->tm_hour, local->tm_min);
    }
}

void Clock::draw() const {
    text.write(clockString, Fonts::IntroDescription, false, 1.5f, -0.94f, -0.92f);
}
