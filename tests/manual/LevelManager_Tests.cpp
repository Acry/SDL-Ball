// LevelManager_Tests.cpp
#include <cstdlib>
#include <thread>
#include <vector>

#include "DisplayManager.hpp"
#include "LevelManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "EventManager.h"
#include "config.h"
#include "EventDispatcher.h"
#include "MouseManager.h"

const float colors[16][4] = {
    {1.0f, 1.0f, 1.0f, 1.0f}, // Weiß
    {0.0f, 0.4f, 1.0f, 1.0f}, // Blau
    {1.0f, 1.0f, 0.0f, 1.0f}, // Gelb
    {1.0f, 0.0f, 0.0f, 1.0f}, // Rot
    {0.0f, 1.0f, 0.0f, 1.0f}, // Grün
    {1.0f, 0.0f, 1.0f, 1.0f}, // Magenta
    {0.0f, 1.0f, 1.0f, 1.0f}, // Cyan
    {1.0f, 0.5f, 0.0f, 1.0f}, // Orange
    {0.5f, 0.5f, 0.5f, 1.0f}, // Grau
    {0.6f, 0.3f, 0.0f, 1.0f}, // Braun
    {0.5f, 0.0f, 0.5f, 1.0f}, // Violett
    {0.0f, 0.0f, 0.5f, 1.0f}, // Dunkelblau
    {0.0f, 0.5f, 0.0f, 1.0f}, // Dunkelgrün
    {0.5f, 0.5f, 0.0f, 1.0f}, // Oliv
    {0.5f, 0.0f, 0.0f, 1.0f}, // Dunkelrot
    {0.0f, 0.0f, 0.0f, 1.0f} // Schwarz
};

std::vector<BrickInfo> bricks;

void onLevelLoaded(const LevelData &data) {
    bricks = data.bricks;
}

int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Level Test");

    const std::filesystem::path themePath = "../themes/default";

    TextManager textManager;
    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Error loading font theme %s", themePath.c_str());
        return EXIT_FAILURE;
    }
    const EventDispatcher eventDispatcher(&eventManager);
    TestHelper testHelper(textManager, &eventManager);

    eventManager.addListener(GameEvent::LevelLoaded,
                             [](const LevelData &data) { onLevelLoaded(data); }, nullptr);

    LevelManager levelManager(&eventManager);
    if (!levelManager.setTheme(themePath)) {
        SDL_Log("Error setting level theme %s", themePath.c_str());
    }

    size_t currentLevel = 1;
    levelManager.loadLevel(currentLevel);

    auto levelCount = levelManager.getLevelCount();
    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
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
    const std::chrono::microseconds targetFrameTime(16667); // ~60 FPS (1s/60 = 16.67ms)
    while (running) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        auto currentTime = frameStart;
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

        int idx = 0;
        for (auto &brick: bricks) {
            glColor4fv(colors[idx % 16]);
            glBegin(GL_QUADS);
            glVertex3f(brick.x, brick.y, 0.0f);
            glVertex3f(brick.x + BRICK_WIDTH, brick.y, 0.0f);
            glVertex3f(brick.x + BRICK_WIDTH, brick.y + BRICK_HEIGHT, 0.0f);
            glVertex3f(brick.x, brick.y + BRICK_HEIGHT, 0.0f);
            glEnd();
            ++idx;
        }
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        char tempText[64];
        sprintf(tempText, "Level: %lu", currentLevel);
        textManager.write(tempText, Fonts::Highscore, true, 1.0f, 0.0f, -0.5f);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(displayManager.sdlWindow);
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);

        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }
    }
    return EXIT_SUCCESS;
}
