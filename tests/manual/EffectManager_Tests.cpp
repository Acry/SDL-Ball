// EffectManager_Tests.cpp
#include <cstdlib>
#include "DisplayManager.hpp"
#include "EffectFactory.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

class TestEffectManager {
    EventManager *eventManager;
    EffectFactory *effectFactory;
    std::vector<std::unique_ptr<ParticleEffect> > effects;

public:
    explicit TestEffectManager(EventManager *eventMgr, EffectFactory *effectFact)
        : eventManager(eventMgr), effectFactory(effectFact) {
    }

    void registerEventListeners() {
        eventManager->addListener(GameEvent::BallHitBrick,
                                  [this](const EventData &data) { handleBallBrickCollision(data); }, this);

        eventManager->addListener(GameEvent::BallHitPaddle,
                                  [this](const EventData &data) { handleBallPaddleCollision(data); }, this);

        eventManager->addListener(GameEvent::PaddleDestroyed,
                                  [this](const EventData &data) { handlePaddleDestroyed(data); }, this);
    }

    [[nodiscard]] const std::vector<std::unique_ptr<ParticleEffect> > &getEffects() const {
        return effects;
    }

private:
    void handleBallBrickCollision(const EventData &data) {
        auto effect = effectFactory->createSparkEffect({data.posX, data.posY});
        effects.push_back(std::move(effect));
    }

    void handleBallPaddleCollision(const EventData &data) {
        auto effect = effectFactory->createSparkleEffect({data.posX, data.posY});
        effects.push_back(std::move(effect));
    }

    void handlePaddleDestroyed(const EventData &data) {
        auto effect = effectFactory->createExplosionEffect({data.posX, data.posY});
        effects.push_back(std::move(effect));
    }
};

class EffectManagerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<EffectFactory> effectFactory;
    std::unique_ptr<TestEffectManager> testEffectManager;

    EffectManagerTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: EffectManager Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
        effectFactory = std::make_unique<EffectFactory>(textureManager.get());
        testEffectManager = std::make_unique<TestEffectManager>(&eventManager, effectFactory.get());
        testEffectManager->registerEventListeners();
    }
};

class EffectManagerTestHelper final : public TestHelper {
    EffectManagerTestContext &ctx;

public:
    explicit EffectManagerTestHelper(EffectManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        const position mousePos = {m_mouseX, m_mouseY};
        if (data.key >= SDLK_1 && data.key <= SDLK_6) {
            const EventData eventData{mousePos.x, mousePos.y};
            switch (data.key) {
                case SDLK_1:
                    ctx.eventManager.emit(GameEvent::BallHitBrick, eventData);
                    break;
                case SDLK_2:
                    ctx.eventManager.emit(GameEvent::BallHitPaddle, eventData);
                    break;
                case SDLK_3:
                    ctx.eventManager.emit(GameEvent::PaddleDestroyed, eventData);
                    break;
                case SDLK_4:
                    ctx.eventManager.emit(GameEvent::BrickDestroyed, eventData);
                    break;
                case SDLK_5:
                    ctx.eventManager.emit(GameEvent::PowerUpCollected, eventData);
                    break;
                case SDLK_6:
                    ctx.eventManager.emit(GameEvent::BallLost, eventData);
                    break;
                default:
                    break;
            }
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto &effect: ctx.testEffectManager->getEffects()) {
            effect->update(deltaTime);
            effect->draw();
        }
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        EffectManagerTestContext ctx;
        EventDispatcher eventDispatcher(&ctx.eventManager);
        EffectManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "1: BallHitBrick-Event",
            "2: BallHitWall-Event",
            "3: PaddleDestroyed-Event",
            "M: Draw Mouse Coordinates",
            "S: Screenshot",
            "ESC: Beenden"
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
