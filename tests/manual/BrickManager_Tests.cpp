// tests/manual/BrickManager_Tests.cpp
#include "BrickManager.h"
#include "LevelManager.h"
#include "Display.hpp"
#include "TestHelper.h"
#include "TextureManager.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: BrickManager Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Error loading font theme");
        return EXIT_FAILURE;
    }
    TestHelper testHelper(textManager);
    EventManager eventManager;
    TextureManager textureManager;
    if (!textureManager.setSpriteTheme("../themes/default")) {
        SDL_Log("Error loading texture theme");
        return EXIT_FAILURE;
    }

    BrickManager brickManager(&eventManager, &textureManager);
    LevelManager levelManager(&eventManager);
    if (!levelManager.setTheme("../themes/default")) {
        SDL_Log("Error setting level theme");
        return EXIT_FAILURE;
    }
    size_t currentLevel = 1;
    BrickData data = levelManager.getBrickDataForLevel(currentLevel);

    brickManager.setupBricks(data);
    auto levelCount = levelManager.getLevelCount();
    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
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
                    display.resize(event.window.data1, event.window.data2);
                }
            }
            if (event.type == SDL_KEYDOWN) {
               switch (event.key.keysym.sym) {
                    case SDLK_s:
                        if (!std::filesystem::exists(screenshotPath)) {
                            std::filesystem::create_directories(screenshotPath);
                        }
                        if (display.screenshot(screenshotPath)) {
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
                            data = levelManager.getBrickDataForLevel(currentLevel);
                        brickManager.setupBricks(data);
                        }
                        break;
                    case SDLK_HOME:
                        currentLevel = 1; {
                        data = levelManager.getBrickDataForLevel(currentLevel);
                        brickManager.setupBricks(data);
                        }
                        break;
                    case SDLK_END:
                        currentLevel = levelCount; {
                         data = levelManager.getBrickDataForLevel(currentLevel);
                        brickManager.setupBricks(data);
                        }
                        break;
                    case SDLK_LEFT:
                        currentLevel--;
                        if (currentLevel < 1) currentLevel = levelCount; {
                        data = levelManager.getBrickDataForLevel(currentLevel);
                        brickManager.setupBricks(data);
                        }
                        break;
                    case SDLK_RIGHT:
                        currentLevel++;
                        if (currentLevel > levelCount) currentLevel = 1; {
                        data = levelManager.getBrickDataForLevel(currentLevel);
                        brickManager.setupBricks(data);
                        }
                        break;
                    case SDLK_ESCAPE:
                        // Spiel beenden
                        running = false;
                        break;
                    default: ;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * (
                                       2.0f / display.viewportW);
                normalizedMouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 * (
                                       2.0f / display.viewportH);
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
        SDL_GL_SwapWindow(display.sdlWindow);
    }

    return EXIT_SUCCESS;
}
