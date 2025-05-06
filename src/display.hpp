#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <SDL2/SDL.h>

class displayClass {
    SDL_GLContext glcontext;
    int displayToUse;

public:
    SDL_Window *sdlWindow;
    int currentW;
    int currentH;
    int numOfDisplays;
    GLfloat glunits_per_xpixel, glunits_per_ypixel;

    bool init(bool, int);

    static bool updateForMenu();

    void resize(int, int);

    void close();
};

#endif /* DISPLAY_HPP_ */
