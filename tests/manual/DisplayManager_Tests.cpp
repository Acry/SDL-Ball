#include <cstdlib>
#include <vector>

#include "DisplayManager.hpp"
#include "TestHelper.h"
#include "TextManager.h"

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Display Test");

    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Error loading font theme");
        return EXIT_FAILURE;
    }
    TestHelper testHelper(textManager);

    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    GLfloat normalizedMouseX = 0.0f, normalizedMouseY = 0.0f;
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    WindowEventData data;
                    data.width = event.window.data1;
                    data.height = event.window.data2;
                    eventManager.emit(GameEvent::WindowResized, data);
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    const std::filesystem::path screenshotPath = "./screenshots/";
                    if (!std::filesystem::exists(screenshotPath)) {
                        std::filesystem::create_directories(screenshotPath);
                    }
                    if (displayManager.screenshot(screenshotPath)) {
                        textManager.addAnnouncement("Screenshot saved.", 1500, Fonts::AnnounceGood);
                    } else {
                        textManager.addAnnouncement("Screenshot not created.", 1500, Fonts::AnnounceBad);
                    }
                }
                if (event.key.keysym.sym == SDLK_m) {
                    testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - displayManager.viewportX - displayManager.viewportW / 2.0f) * (
                                       2.0f / displayManager.viewportW);
                normalizedMouseY = (event.motion.y - displayManager.viewportY - displayManager.viewportH / 2.0f) * -1 *
                                   (
                                       2.0f / displayManager.viewportH);
            }
        }

        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid();
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
