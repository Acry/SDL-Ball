#include <iostream>
#include <epoxy/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include "display.hpp"

#include "settings_manager.h"

using namespace std;

extern settings setting;

bool displayClass::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Fehler bei SDL Video Init: %s", SDL_GetError());
        return false;
    }
    // Nutze display aus settings
    numOfDisplays = SDL_GetNumVideoDisplays();
    if (numOfDisplays > 0 && setting.displayToUse < numOfDisplays) {
        displayToUse = setting.displayToUse;
        // todo apply to settings and flag settings changed?
    } else {
        displayToUse = 0;
    }
    unsigned int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (setting.fullscreen) {
        // Im Vollbildmodus native Auflösung verwenden
        SDL_DisplayMode currentDisplayMode;
        SDL_GetCurrentDisplayMode(displayToUse, &currentDisplayMode);
        currentW = currentDisplayMode.w;
        currentH = currentDisplayMode.h;
        flags |= SDL_WINDOW_FULLSCREEN;
    } else {
        // Im Fenstermodus konfigurierte Auflösung verwenden
        currentW = setting.res_x > 0 ? setting.res_x : 1280; // Fallback-Wert
        currentH = setting.res_y > 0 ? setting.res_y : 720; // Fallback-Wert
    }

    // Nutze die Fullscreen-Einstellung aus settings
    if (setting.fullscreen) {
        // flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;  // Für echten Vollbildmodus
        // Alternativ:
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("\nError: Unable to initialize SDL:%s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    sdlWindow = SDL_CreateWindow("SDL-Ball",
                                 SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
                                 SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
                                 currentW, currentH,
                                 flags);

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

    // Quadrat mit maximaler Höhe, zentriert
    viewportSize = std::min(width, height);
    viewportX = (width - viewportSize) / 2;
    viewportY = (height - viewportSize) / 2;
    glViewport(viewportX, viewportY, viewportSize, viewportSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluPerspective(45.0f, ratio, 0.1f, 100.0f);

    glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection, flipping bottom and top for SDL2

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void displayClass::close() const {
    SDL_DestroyWindow(sdlWindow);
    SDL_GL_DeleteContext(glcontext);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_Quit();
}
