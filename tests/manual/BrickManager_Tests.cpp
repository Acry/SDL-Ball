// tests/manual/BrickManager_Tests.cpp
#include "BrickManager.h"
#include "LevelManager.h"
#include "DisplayManager.hpp"
#include "TestHelper.h"
#include "TextureManager.h"
#include "EventManager.h"
#include "MouseManager.h"

int main() {
    EventManager eventManager;
    // MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: BrickManager Test");

    const std::filesystem::path themePath = "../themes/default";

    TextManager textManager;
    if (!textManager.setTheme(themePath)) {
        SDL_Log("Error loading font theme %s", themePath.c_str());
        return EXIT_FAILURE;
    }

    TestHelper testHelper(textManager, &eventManager);

    TextureManager textureManager;
    if (!textureManager.setSpriteTheme("../themes/default")) {
        SDL_Log("Error loading texture theme");
        return EXIT_FAILURE;
    }


    LevelManager levelManager(&eventManager);
    if (!levelManager.setTheme("../themes/default")) {
        SDL_Log("Error setting level theme");
        return EXIT_FAILURE;
    }
    BrickManager brickManager(&eventManager, &textureManager);

    size_t currentLevel = 1;
    levelManager.loadLevel(currentLevel);

    auto levelCount = levelManager.getLevelCount();
    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "Space - advance level",
        "-> - next level",
        "<- - previous level",
        "Pos1 - first level",
        "End - last level",
        "ESC: Quit"
    };
    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    GLfloat normalizedMouseX = 0.0f, normalizedMouseY = 0.0f;
    const std::filesystem::path screenshotPath = "./screenshots/";
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
                    displayManager.resize(event.window.data1, event.window.data2);
                }
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_s:
                        if (!std::filesystem::exists(screenshotPath)) {
                            std::filesystem::create_directories(screenshotPath);
                        }
                        if (displayManager.screenshot(screenshotPath)) {
                            textManager.addAnnouncement("Screenshot saved.", 1500, Fonts::AnnounceGood);
                        } else {
                            textManager.addAnnouncement("Screenshot not created.", 1500, Fonts::AnnounceBad);
                        }
                        break;
                    case SDLK_m:
                        testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                        break;
                    case SDLK_SPACE:
                        currentLevel++;
                        if (currentLevel > levelCount) currentLevel = 1; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_HOME:
                        currentLevel = 1; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_END:
                        currentLevel = levelCount; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_LEFT:
                        currentLevel--;
                        if (currentLevel < 1) currentLevel = levelCount; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_RIGHT:
                        currentLevel++;
                        if (currentLevel > levelCount) currentLevel = 1; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default: ;
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

        brickManager.draw(deltaTime);
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        char tempText[64];
        sprintf(tempText, "Level: %lu", currentLevel);
        textManager.write(tempText, Fonts::Highscore, true, 1.0f, 0.0f, -0.5f);
        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }

    return EXIT_SUCCESS;
}
