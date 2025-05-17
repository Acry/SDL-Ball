#pragma once
#include <epoxy/gl.h>
#include <filesystem>
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
    Display(const int display, const int width, const int height, const bool fullscreen);
    GLfloat playfield_ratio;
    GLfloat window_ratio;
    static bool updateForMenu();
    [[nodiscard]] bool screenshot(const std::filesystem::path &pathName) const;
    void resize(int, int);
    ~Display();
};
