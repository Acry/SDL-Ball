// DisplayManager_Tests.cpp
#include <cstdlib>
#include <vector>

#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"

int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Display Test");

    TextManager textManager(&eventManager);
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Error loading font theme");
        return EXIT_FAILURE;
    }
    const EventDispatcher eventDispatcher(&eventManager);
    TestHelper testHelper(textManager, &eventManager);

    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        running = eventDispatcher.processEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid();
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
