// Lives.h
#pragma once

#include "TextureManager.h"

class Lives {
    TextureManager *textureManager;
    int lives;

public:
    explicit Lives(TextureManager *textureManager, int initialLives = 3);

    void addLives(int delta);

    [[nodiscard]] int getLives() const;

    void setLives(int value);

    void draw() const;
};
