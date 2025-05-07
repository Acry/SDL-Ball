#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

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

    bool init();

    static bool updateForMenu();

    void resize(int, int);

    void close() const;
};

#endif /* DISPLAY_HPP_ */
