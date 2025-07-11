// Clock.h
#pragma once
#include "TextManager.h"

class Clock {
public:
    explicit Clock(TextManager &textManager);

    void update(float deltaTime);

    void draw() const;

private:
    TextManager &text;
    float timeAccumulator;
    char clockString[13];
};
