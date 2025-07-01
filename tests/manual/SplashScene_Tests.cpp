// SplashScene_Tests.cpp
#include <chrono>
#include <cstdlib>
#include <epoxy/gl.h>
#include <SDL2/SDL.h>

#include "TextManager.h"
#include "DisplayManager.hpp"
#include "SoundManager.h"
#include "SplashScreen.h"
#include "SoundManager.h"
#include "SplashScreen.h"

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: SplashScene Test");
    TextureManager textureManager;
    texture tex;
    textureManager.load("../images/current-logo.png", tex);
    SplashScreen splashScreen(&tex.textureProperties);
    SoundManager soundManager;
    //soundManager.loadMusic("../tests/themes/test/music/Breaking Boundaries.mp3");
    soundManager.loadMusic("../tests/themes/test/music/Pixel Breaker.mp3");
    soundManager.playMusic();
    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        splashScreen.draw();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
