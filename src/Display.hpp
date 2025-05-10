#pragma once

#include <SDL2/SDL.h>

class Display {
    int displayToUse = 0;
    SDL_GLContext glcontext = nullptr;

    static void initGL();
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
    static bool screenshot();
    void resize(int, int);
    ~Display();
};

