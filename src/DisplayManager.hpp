// DisplayManager.h
#pragma once

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <filesystem>

#include "EventManager.h"

class DisplayManager {
    IEventManager *eventManager;
    int displayToUse = 0;
    SDL_GLContext glcontext = nullptr;

    [[nodiscard]] bool initOpenGL(unsigned int flags);

public:
    explicit DisplayManager(IEventManager *eventMgr);

    SDL_Window *sdlWindow = nullptr;
    int currentW;
    int currentH;
    int numOfDisplays;
    GLfloat glunits_per_xpixel, glunits_per_ypixel;
    int viewportX, viewportY, viewportH, viewportW;

    bool init(int display, int width, int height, bool fullscreen);

    void handleWindowResize(const WindowEventData &data);

    GLfloat playfield_ratio;
    GLdouble window_ratio;

    // FIXME: Use TGA format only for Development and testing purposes.
    // Use AVIF for single images or JPEG XL for short sequences.
    [[nodiscard]] bool screenshot(const std::filesystem::path &pathName) const;

    void resize(int, int);

    ~DisplayManager();
};
