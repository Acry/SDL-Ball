// GameManager_Tests.cpp

#include <chrono>
#include <cstdlib>
#include <memory>
#include <vector>

#include "BackgroundManager.h"
#include "BallManager.h"
#include "BrickManager.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "HudCompositor.h"
#include "KeyboardManager.h"
#include "LevelManager.h"
#include "MouseManager.h"
#include "PaddleManager.h"
#include "PlayfieldBorder.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

class TestGameManager {
    IEventManager *eventManager;
    TextManager *textManager;
    TextureManager *textureManager;
    LevelManager *levelManager;
    BackgroundManager *backgroundManager;
    BrickManager brickManager;
    std::unique_ptr<PlayfieldBorder> leftBorder;
    std::unique_ptr<PlayfieldBorder> rightBorder;
    std::unique_ptr<PlayfieldBorder> topBorder;
    SpriteSheetAnimationManager animationManager;
    HudCompositor hudManager;
    std::unique_ptr<BallManager> ballManager;
    std::unique_ptr<PaddleManager> paddleManager;

public:
    TestGameManager(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager)
        : eventManager(eventManager),
          textManager(textManager),
          textureManager(textureManager),
          levelManager(new LevelManager(eventManager)),
          backgroundManager(new BackgroundManager(*textureManager)),

          brickManager(eventManager, textureManager, &animationManager),
          hudManager(eventManager, textManager, textureManager) {
        backgroundManager->registerEvents(eventManager);
        levelManager->setTheme("../themes/default");

        auto *borderTex = textureManager->getMiscTexture(MiscTexture::Border);
        leftBorder = std::make_unique<PlayfieldBorder>(PlayfieldBorder::Side::Left, *borderTex, eventManager);
        rightBorder = std::make_unique<PlayfieldBorder>(PlayfieldBorder::Side::Right, *borderTex, eventManager);
        topBorder = std::make_unique<PlayfieldBorder>(PlayfieldBorder::Side::Top, *borderTex, eventManager);
        levelManager->loadLevel(1);
        ballManager = std::make_unique<BallManager>(eventManager, textureManager, &animationManager);
        paddleManager = std::make_unique<PaddleManager>(eventManager, textureManager, &animationManager);
        paddleManager->spawn();
        const float paddleY = paddleManager->activePaddle->getPosY();
        const float paddleHeight = paddleManager->activePaddle->getHeight();
        ballManager->spawn(0.0f, paddleY - paddleHeight, true);
    }

    void update(const float deltaTime) {
        animationManager.updateAllAnimations(deltaTime);
        topBorder->update(deltaTime);
        hudManager.update(deltaTime);
        ballManager->update(deltaTime);
        updateGluedBalls();
        paddleManager->update(deltaTime);
    }

    void draw() {
        backgroundManager->draw();
        leftBorder->draw();
        rightBorder->draw();
        topBorder->draw();
        brickManager.draw();
        ballManager->draw();
        paddleManager->draw();
        hudManager.draw();
    }

    void resetGame() {
    }

    void updateGluedBalls() {
        if (!paddleManager->activePaddle) return;

        float paddleX = paddleManager->activePaddle->getPosX();
        float paddleY = paddleManager->activePaddle->getPosY();
        float paddleWidth = paddleManager->activePaddle->getWidth();
        float paddleHeight = paddleManager->activePaddle->getHeight();

        for (auto *ball: ballManager->managedObjects) {
            if (ball->isGlued()) {
                // Ball im Zentrum des Paddles positionieren
                ball->centerX = paddleX + paddleWidth / 2.0f;
                // Halbe Ballhöhe addieren
                ball->centerY = paddleY + paddleHeight + ball->height / 2.0f;

                // pos_x und pos_y aktualisieren
                ball->pos_x = ball->centerX - ball->width / 2.0f;
                ball->pos_y = ball->centerY - ball->height / 2.0f;
            }
        }
    }
};

class GameManagerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<TestGameManager> gameManager;


    GameManagerTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: GameManager Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        textureManager->setSpriteTheme("../themes/default");
        textureManager->setBackgroundTheme("../themes/default");
        gameManager = std::make_unique<TestGameManager>(&eventManager, &textManager, textureManager.get());
    }
};

class GameManagerTestHelper final : public TestHelper {
    GameManagerTestContext &ctx;

public:
    explicit GameManagerTestHelper(GameManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);

        switch (data.key) {
            case SDLK_1:
                break;
            case SDLK_2:
                break;
            case SDLK_c:
                break;
            case SDLK_r:
                break;
            default:
                break;
        }
    }

    void update(const float deltaTime) const {
        ctx.gameManager->update(deltaTime);
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();

        ctx.gameManager->draw();

        renderInstructions(deltaTime, instructions);

        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        GameManagerTestContext ctx;
        EventDispatcher eventDispatcher(&ctx.eventManager);
        GameManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "ESC: Quit",
            "M: Toggle Mouse Coordinates",
            "S: Screenshot"
        };

        bool running = true;
        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            running = eventDispatcher.processEvents();

            testHelper.update(deltaTime);
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}