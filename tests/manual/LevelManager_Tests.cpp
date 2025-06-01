#include <cstdlib>
#include <vector>

#include "Display.hpp"
#include "LevelManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "Brick.h"

const float colors[16][4] = {
    {1.0f, 1.0f, 1.0f, 1.0f},   // Weiß
    {0.0f, 0.4f, 1.0f, 1.0f},   // Blau
    {1.0f, 1.0f, 0.0f, 1.0f},   // Gelb
    {1.0f, 0.0f, 0.0f, 1.0f},   // Rot
    {0.0f, 1.0f, 0.0f, 1.0f},   // Grün
    {1.0f, 0.0f, 1.0f, 1.0f},   // Magenta
    {0.0f, 1.0f, 1.0f, 1.0f},   // Cyan
    {1.0f, 0.5f, 0.0f, 1.0f},   // Orange
    {0.5f, 0.5f, 0.5f, 1.0f},   // Grau
    {0.6f, 0.3f, 0.0f, 1.0f},   // Braun
    {0.5f, 0.0f, 0.5f, 1.0f},   // Violett
    {0.0f, 0.0f, 0.5f, 1.0f},   // Dunkelblau
    {0.0f, 0.5f, 0.0f, 1.0f},   // Dunkelgrün
    {0.5f, 0.5f, 0.0f, 1.0f},   // Oliv
    {0.5f, 0.0f, 0.0f, 1.0f},   // Dunkelrot
    {0.0f, 0.0f, 0.0f, 1.0f}    // Schwarz
};

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: Level Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Error loading font theme");
        return EXIT_FAILURE;
    }
    TestHelper testHelper(textManager);
    EventManager eventManager;
    LevelManager levelManager(&eventManager);
    if (!levelManager.setTheme("../themes/default")) {
        SDL_Log("Error setting level theme");
    }
    size_t currentLevel = 1;
    std::vector<Brick> bricks;
    LevelData levelInfo;
    auto [newBricks, newLevelInfo] = levelManager.getBrickDataForLevel(currentLevel);
    bricks = std::move(newBricks);
    levelInfo = std::move(newLevelInfo);
    auto levelCount = levelManager.getLevelCount();
    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "Space - advance level",
        "Pos1 - first level",
        "End - last level",
        "r - reset level",
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
                        if (currentLevel > levelCount) currentLevel = 1;
                    {
                        auto [newBricks, newLevelInfo] = levelManager.getBrickDataForLevel(currentLevel);
                        bricks = std::move(newBricks);
                        levelInfo = std::move(newLevelInfo);
                    }
                        break;
                    case SDLK_HOME:
                        currentLevel = 1;
                    {
                        auto [newBricks, newLevelInfo] = levelManager.getBrickDataForLevel(currentLevel);
                        bricks = std::move(newBricks);
                        levelInfo = std::move(newLevelInfo);
                    }
                        break;
                    case SDLK_END:
                        currentLevel = levelCount;
                    {
                        auto [newBricks, newLevelInfo] = levelManager.getBrickDataForLevel(currentLevel);
                        bricks = std::move(newBricks);
                        levelInfo = std::move(newLevelInfo);
                    }
                        break;
                    case SDLK_r:
                    {
                        auto [newBricks, newLevelInfo] = levelManager.getBrickDataForLevel(currentLevel);
                        bricks = std::move(newBricks);
                        levelInfo = std::move(newLevelInfo);
                    }
                        break;
                    case SDLK_ESCAPE:
                        // Spiel beenden
                        running = false;
                        break;
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

        int idx = 0;
        for (auto &brick : bricks) {
            glColor4fv(colors[idx % 16]);
            glBegin(GL_QUADS);
            glVertex3f(brick.pos_x, brick.pos_y, 0.0f);
            glVertex3f(brick.pos_x + brick.width, brick.pos_y, 0.0f);
            glVertex3f(brick.pos_x + brick.width, brick.pos_y + brick.height, 0.0f);
            glVertex3f(brick.pos_x, brick.pos_y + brick.height, 0.0f);
            glEnd();
            ++idx;
        }
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
