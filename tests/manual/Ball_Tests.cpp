// Ball_Tests.cpp
#include <cstdlib>
#include <numeric>
#include <vector>
#include <cmath>
#include <algorithm>
#include "BallManager.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "GrowableObject.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"

class BallTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    SpriteSheetAnimationManager animationManager;
    std::unique_ptr<BallManager> ballManager;

    BallTestContext()
        : mouseManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Ball Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
        ballManager = std::make_unique<BallManager>(&eventManager, textureManager.get(), &animationManager);
    }
};

class BallTestHelper final : public TestHelper {
    BallTestContext &ctx;

public:
    explicit BallTestHelper(BallTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_e:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    const bool currentState = ctx.ballManager->selectedBall->isExplosive();
                    ctx.ballManager->setExplosive(ctx.ballManager->selectedBall, !currentState);
                }
                break;
            case SDLK_g:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    const float currentSize = ctx.ballManager->selectedBall->getWidth();
                    float newSize = currentSize * 1.2f;
                    if (newSize <= 0.2f) {
                        ctx.ballManager->selectedBall->grow(ctx.ballManager->selectedBall->getWidth() * 1.2f);
                    }
                }
                break;
            case SDLK_k:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    const float currentSize = ctx.ballManager->selectedBall->width;
                    float newSize = currentSize * 0.8f;
                    if (newSize >= 0.01f) {
                        ctx.ballManager->selectedBall->grow(ctx.ballManager->selectedBall->getWidth() * 0.8f);
                    }
                }
                break;
            case SDLK_DELETE:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    Ball *currentBall = ctx.ballManager->selectedBall;
                    ctx.ballManager->despawnBall(currentBall);
                    ctx.ballManager->selectedBall = nullptr;

                    // Suche nach dem nächsten aktiven Ball
                    for (auto *ball: ctx.ballManager->managedObjects) {
                        if (ball->isPhysicallyActive()) {
                            ctx.ballManager->selectedBall = ball;
                            break;
                        }
                    }
                }
            default:
                break;
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
            if (ctx.ballManager) {
                ctx.ballManager->spawn(m_mouseX, m_mouseY, false);
            }
        }
    }

    void MouseWheelScrolled(const MouseEventData &data) override {
        TestHelper::MouseWheelScrolled(data);
        if (ctx.ballManager->managedObjects.empty()) {
            return;
        }
        std::vector<Ball *> activeBalls;
        for (auto *ball: ctx.ballManager->managedObjects) {
            if (ball->isPhysicallyActive()) {
                activeBalls.push_back(ball);
            }
        }

        if (activeBalls.empty()) {
            return;
        }
        const auto currentBallIt = std::find(activeBalls.begin(), activeBalls.end(), ctx.ballManager->selectedBall);
        size_t currentIndex = 0;

        if (currentBallIt != activeBalls.end()) {
            currentIndex = std::distance(activeBalls.begin(), currentBallIt);
        }

        // Berechne den nächsten Ball-Index basierend auf der Rad-Bewegung
        int nextIndex;
        if (data.wheelY < 0) {
            // Nach unten scrollen (vorwärts)
            nextIndex = (currentIndex + 1) % activeBalls.size();
        } else if (data.wheelY > 0) {
            // Nach oben scrollen (rückwärts)
            nextIndex = (currentIndex + activeBalls.size() - 1) % activeBalls.size();
        } else {
            // Keine vertikale Bewegung
            return;
        }

        ctx.ballManager->selectedBall = activeBalls[nextIndex];
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();
        ctx.animationManager.updateAllAnimations(deltaTime);
        ctx.ballManager->update(deltaTime);
        ctx.ballManager->draw();
        renderInstructions(deltaTime, instructions);
        drawBallCount();
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }

    void drawBallCount() const {
        int activeBalls = 0;
        for (const auto *ball: ctx.ballManager->managedObjects) {
            if (ball->isPhysicallyActive()) {
                activeBalls++;
            }
        }
        char ballCountText[32];
        sprintf(ballCountText, "Balls: %d", activeBalls);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ctx.textManager.write(ballCountText, Fonts::Highscore, true, 0.4f, 0.0f, -0.9f);
    }
};

int main() {
    try {
        BallTestContext ctx;
        BallTestHelper testHelper(ctx);
        EventDispatcher eventDispatcher(&ctx.eventManager);
        const std::vector<std::string> instructions = {
            "LMB: Spawn Ball",
            "DEL: Despawn Ball",
            "Vertical Mouse Wheel: Select Ball",
            "E: Toggle Explosive State of Selected Ball",
            "G: Grow Selected Ball",
            "K: Shrink Selected Ball",
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
