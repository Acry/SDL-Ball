#include <iostream>
#include <epoxy/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include "display.hpp"

using namespace std;

bool displayClass::init(const bool fullscreen, const int displayToUseIn) {
    int SDL_videomodeSettings = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    //Save the current resolution so it can be restored at exit
    // Declare display mode structure to be filled in.
    SDL_DisplayMode currentDisplayMode;

    SDL_Init(SDL_INIT_VIDEO);

    // Get the current display mode of all displays.
    numOfDisplays = SDL_GetNumVideoDisplays();
    SDL_Rect displayBounds[numOfDisplays] = {0};

    for (int i = 0; i < numOfDisplays; ++i) {
        if (const int should_be_zero = SDL_GetCurrentDisplayMode(i, &currentDisplayMode); should_be_zero != 0) {
            // In case of error...
            SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
        } else {
            // On success, print the current display mode.
            SDL_GetDisplayBounds(i, &displayBounds[i]);
            auto [x, y, w, h] = displayBounds[i];
            SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. %d %d %d %d",
                    i, currentDisplayMode.w, currentDisplayMode.h, currentDisplayMode.refresh_rate,
                    x, y, h, w);
        }
    }

    if (numOfDisplays > 0 && displayToUseIn < numOfDisplays) {
        displayToUse = displayToUseIn;
    } else {
        displayToUse = 0;
    }

    currentW = displayBounds[displayToUse].w;
    currentH = displayBounds[displayToUse].h;

    //Initialize SDL
#ifndef NOSOUND
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
#else
  if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_JOYSTICK) <0 )
#endif
    {
        printf("\nError: Unable to initialize SDL:%s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    if (fullscreen) {
        // SDL_videomodeSettings |= SDL_WINDOW_FULLSCREEN_DESKTOP;  // Für echten Vollbildmodus
        // Alternativ:
        SDL_videomodeSettings |= SDL_WINDOW_FULLSCREEN;
    }

    sdlWindow = SDL_CreateWindow("SDL-Ball",
                                 SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
                                 SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
                                 displayBounds[displayToUse].w, displayBounds[displayToUse].h,
                                 SDL_videomodeSettings);

    glcontext = SDL_GL_CreateContext(sdlWindow);

    if ((sdlWindow == nullptr) || (glcontext == nullptr)) {
        SDL_Log("Error:%s", SDL_GetError());
        return false;
    }
    int maj, min;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
    SDL_Log("=== Active OpenGL Context ===");
    SDL_Log("Context Version: %d.%d", maj, min);


    int doubleBuffered = 0;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffered);
    SDL_Log("Double buffering %s", doubleBuffered ? "aktiviert" : "deaktiviert");
    resize(currentW, currentH);
    return true;
}

bool displayClass::updateForMenu() {
    return true;
}

void displayClass::resize(const int width, const int height) {
    if (height == 0)
       return;

    currentW = width;
    currentH = height;

    const GLfloat ratio = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
    glunits_per_xpixel = 2.485281374 * ratio / currentW;
    glunits_per_ypixel = 2.485281374 / currentH;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, ratio, 0.1f, 10.0f);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

void displayClass::close() {
    SDL_DestroyWindow(sdlWindow);
    SDL_GL_DeleteContext(glcontext);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_Quit();
}
