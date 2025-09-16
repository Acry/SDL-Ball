// Paddle_Tests.cpp
#include <cstdlib>
#include <vector>
#include <cmath>
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "GrowableObject.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "Paddle.h"
#include "PaddleManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"

class PaddleTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    SpriteSheetAnimationManager animationManager;
    std::unique_ptr<PaddleManager> paddleManager;

    PaddleTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Paddle Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
        paddleManager = std::make_unique<PaddleManager>(&eventManager, textureManager.get(),
                                                        &animationManager);
    }
};

class PaddleTestHelper final : public TestHelper {
    PaddleTestContext &ctx;

public:
    explicit PaddleTestHelper(PaddleTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_w:
                if (!ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->spawn();
                }
                break;
            case SDLK_DELETE:
                if (ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->despawn();
                }
                break;
            case SDLK_g:
                if (ctx.paddleManager->activePaddle) {
                    const float currentSize = ctx.paddleManager->activePaddle->getWidth();
                    float newSize = currentSize * 1.2f;
                    if (newSize <= 0.35f) {
                        ctx.paddleManager->activePaddle->grow(
                            ctx.paddleManager->activePaddle->getWidth() * 1.2f);
                    }
                }
                break;
            case SDLK_k:
                if (ctx.paddleManager->activePaddle) {
                    const float currentSize = ctx.paddleManager->activePaddle->getWidth();
                    float newSize = currentSize * 0.8f;
                    if (newSize >= 0.1f) {
                        ctx.paddleManager->activePaddle->grow(
                            ctx.paddleManager->activePaddle->getWidth() * 0.8f);
                    }
                }
                break;
            case SDLK_u:
                if (ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->setGlueLayer(!ctx.paddleManager->activePaddle->getGlueLayer());
                }
                break;
            case SDLK_l:
                if (ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->setGunLayer(!ctx.paddleManager->activePaddle->getGunLayer());
                }
            default:
                break;
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();
        ctx.animationManager.updateAllAnimations(deltaTime);
        ctx.paddleManager->update(deltaTime);
        ctx.paddleManager->draw();
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        PaddleTestContext ctx;
        PaddleTestHelper testHelper(ctx);
        EventDispatcher eventDispatcher(&ctx.eventManager);
        const std::vector<std::string> instructions = {
            "W: Spawn Paddle",
            "DEL: DeSpawn Paddle",
            "G: Grow Paddle",
            "K: Shrink Paddle",
            "LMB: Fire Gun, when Paddle has Gun Layer",
            "U: Toggle Glue State of Paddle",
            "L: Toggle Laser State of Paddle",
            "S: Screenshot",
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
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}