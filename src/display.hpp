#pragma once

#include <SDL2/SDL.h>

class displayClass {
    SDL_GLContext glcontext = nullptr;
    int displayToUse = 0;

public:
    SDL_Window *sdlWindow = nullptr;
    int currentW;
    int currentH;
    int numOfDisplays;
    GLfloat glunits_per_xpixel, glunits_per_ypixel;
    int viewportX, viewportY, viewportH, viewportW;
    bool init();
    GLfloat playfield_ratio;
    GLfloat window_ratio;
    static bool updateForMenu();

    void resize(int, int);
    ~displayClass();
};

