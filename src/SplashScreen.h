# pragma once

// texture Manager->load("splashscreen.png");
// draw

#include "TextureManager.h"

class SplashScreen {
public:
    explicit SplashScreen(TextureResource *texture);

    void draw() const;

private:
    TextureResource *textureProperties;
};
