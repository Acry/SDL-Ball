// LevelManager_Tests.cpp
#include <cstdlib>
#include <thread>
#include <vector>

#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "EventManager.h"
#include "LevelManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "config.h"

const GLfloat colors[16][4] = {
    {1.0f, 1.0f, 1.0f, 1.0f}, // Weiß, 0
    {0.0f, 0.4f, 1.0f, 1.0f}, // Blau
    {1.0f, 1.0f, 0.0f, 1.0f}, // Gelb
    {1.0f, 0.0f, 0.0f, 1.0f}, // Rot
    {0.0f, 1.0f, 0.0f, 1.0f}, // Grün
    {1.0f, 0.0f, 1.0f, 1.0f}, // Magenta, 5
    {0.0f, 1.0f, 1.0f, 1.0f}, // Cyan
    {1.0f, 0.5f, 0.0f, 1.0f}, // Orange
    {0.5f, 0.5f, 0.5f, 1.0f}, // Grau
    {0.6f, 0.3f, 0.0f, 1.0f}, // Braun
    {0.5f, 0.0f, 0.5f, 1.0f}, // Violett, 10
    {0.0f, 0.0f, 0.5f, 1.0f}, // Dunkelblau
    {0.0f, 0.5f, 0.0f, 1.0f}, // Dunkelgrün
    {1.0f, 1.0f, 1.0f, 0.5f},
    {1.0f, 1.0f, 1.0f, 0.2f},
    {0.0f, 0.0f, 0.0f, 1.0f} // Schwarz, 15
};

int getBrickColorIndex(const BrickType type) {
    switch (type) {
        case BrickType::Blue: return 1;
        case BrickType::Yellow: return 2;
        case BrickType::Cement: return 11;
        case BrickType::Glass: return 13;
        case BrickType::Green: return 4;
        case BrickType::Grey: return 8;
        case BrickType::Purple: return 5;
        case BrickType::White: return 0;
        case BrickType::Invisible: return 14;
        case BrickType::Red: return 3;
        case BrickType::Explosive: return 7;
        case BrickType::Doom: return 10;
        case BrickType::Base: return 12;
        default: return 15;
    }
}

struct LevelTestContext {
    EventManager eventManager;
    MouseManager mouseManager;
    DisplayManager displayManager;
    TextManager textManager;
    LevelManager levelManager;

    std::vector<BrickInfo> bricks;
    size_t currentLevel = 1;
    size_t levelCount = 0;

    LevelTestContext()
        : mouseManager(&eventManager),
          displayManager(&eventManager),
          levelManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Level Test");

        textManager.setTheme("../tests/themes/test");

        eventManager.addListener(GameEvent::LevelLoaded,
                                 [this](const LevelData &data) {
                                     this->bricks = data.bricks;
                                 }, nullptr);

        if (!levelManager.setTheme("../themes/default")) {
            throw std::runtime_error("Error setting level theme");
        }
        levelCount = levelManager.getLevelCount();
        levelManager.loadLevel(currentLevel);
    }

    void loadLevel(size_t level) {
        if (level < 1) level = levelCount;
        if (level > levelCount) level = 1;

        currentLevel = level;
        levelManager.loadLevel(currentLevel);
    }
};

class LevelTestHelper final : public TestHelper {
    LevelTestContext &ctx;

public:
    explicit LevelTestHelper(LevelTestContext &context)
        : TestHelper(context.textManager, &context.eventManager),
          ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);

        switch (data.key) {
            case SDLK_RIGHT:
            case SDLK_SPACE:
                ctx.loadLevel(ctx.currentLevel + 1);
                break;
            case SDLK_LEFT:
                ctx.loadLevel(ctx.currentLevel - 1);
                break;
            case SDLK_HOME:
                ctx.loadLevel(1);
                break;
            case SDLK_END:
                ctx.loadLevel(ctx.levelCount);
                break;
            default:
                break;
        }
    }

    void renderLevel(const float deltaTime, const std::vector<std::string> &instructions) const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawGrid();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (const auto &[type, x, y, CustomBrickColor]: ctx.bricks) {
            if (CustomBrickColor.r != 0.0f && CustomBrickColor.g != 0.0f &&
                CustomBrickColor.b != 0.0f && CustomBrickColor.a != 0.0f) {
                glColor4f(CustomBrickColor.r,
                          CustomBrickColor.g,
                          CustomBrickColor.b,
                          CustomBrickColor.a);
            } else {
                const int colorIdx = getBrickColorIndex(type);
                glColor4fv(colors[colorIdx]);
            }
            // Fill
            glBegin(GL_QUADS);
            glVertex3f(x, y, 0.0f);
            glVertex3f(x + BRICK_WIDTH, y, 0.0f);
            glVertex3f(x + BRICK_WIDTH, y + BRICK_HEIGHT, 0.0f);
            glVertex3f(x, y + BRICK_HEIGHT, 0.0f);
            glEnd();

            // Outline
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            glLineWidth(0.1f);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x, y, 0.0f);
            glVertex3f(x + BRICK_WIDTH, y, 0.0f);
            glVertex3f(x + BRICK_WIDTH, y + BRICK_HEIGHT, 0.0f);
            glVertex3f(x, y + BRICK_HEIGHT, 0.0f);
            glEnd();
        }
        glDisable(GL_BLEND);

        drawCenterLines();
        renderInstructions(deltaTime, instructions);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        char tempText[64];
        sprintf(tempText, "Level: %lu", ctx.currentLevel);
        ctx.textManager.write(tempText, Fonts::Highscore, true, 1.0f, 0.0f, -0.5f);

        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        LevelTestContext ctx;
        const LevelTestHelper testHelper(ctx);
        const EventDispatcher eventDispatcher(&ctx.eventManager);

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

        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            running = eventDispatcher.processEvents();
            testHelper.renderLevel(deltaTime, instructions);
        }
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
