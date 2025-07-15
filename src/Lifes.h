// Lifes.h
#pragma once

#include "TextureManager.h"

class Lifes {
    TextureManager *textureManager;
    int lives;

public:
    explicit Lifes(TextureManager *textureManager, int initialLives = 3);

    void addLives(int delta);

    int getLives() const;

    void setLives(int value);

    void draw() const;
};
