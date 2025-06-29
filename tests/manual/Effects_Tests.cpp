// Effects_Tests.cpp
#include <cstdlib>
#include <vector>
#include <cmath>
#include "DisplayManager.hpp"
#include "EffectFactory.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

class EffectsTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<EffectFactory> effectFactory;

    EffectsTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Tracer Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
        effectFactory = std::make_unique<EffectFactory>(textureManager.get());
    }
};

class EffectsTestHelper final : public TestHelper {
    EffectsTestContext &ctx;
    std::unique_ptr<ParticleEffect> currentEffect;
    int currentEffectType = 0;

    const std::vector<std::string> effectNames = {
        "None", "Spark", "Fire", "Smoke", "Rain", "Explosion", "Sparkle"
    };

public:
    explicit EffectsTestHelper(EffectsTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context),
          currentEffect(nullptr) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        const position mousePos = {m_mouseX, m_mouseY};
        if (data.key >= SDLK_0 && data.key <= SDLK_6) {
            switch (data.key) {
                case SDLK_0:
                    currentEffectType = 0;
                    currentEffect = nullptr;
                    break;
                case SDLK_1:
                    currentEffectType = 1;
                    currentEffect = ctx.effectFactory->createSparkEffect(mousePos);
                    break;
                case SDLK_2:
                    currentEffectType = 2;
                    currentEffect = ctx.effectFactory->createFireEffect(mousePos);
                    break;
                case SDLK_3:
                    currentEffectType = 3;
                    currentEffect = ctx.effectFactory->createSmokeEffect(mousePos);
                    break;
                case SDLK_4:
                    currentEffectType = 4;
                    currentEffect = ctx.effectFactory->createRainEffect(mousePos);
                    break;
                case SDLK_5:
                    currentEffectType = 5;
                    currentEffect = ctx.effectFactory->createExplosionEffect(mousePos);
                    break;
                case SDLK_6:
                    currentEffectType = 6;
                    currentEffect = ctx.effectFactory->createSparkleEffect(mousePos);
                    break;

                default:
                    break;
            }
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT && currentEffectType > 0) {
            const position mousePos = {m_mouseX, m_mouseY};
            switch (currentEffectType) {
                case 1: currentEffect = ctx.effectFactory->createSparkEffect(mousePos);
                    break;
                case 2: currentEffect = ctx.effectFactory->createFireEffect(mousePos);
                    break;
                case 3: currentEffect = ctx.effectFactory->createSmokeEffect(mousePos);
                    break;
                case 4: currentEffect = ctx.effectFactory->createRainEffect(mousePos);
                    break;
                case 5: currentEffect = ctx.effectFactory->createExplosionEffect(mousePos);
                    break;
                case 6: currentEffect = ctx.effectFactory->createSparkleEffect(mousePos);
                    break;
                default: ;
            }
        }
    }

    void MouseWheelScrolled(const MouseEventData &data) override {
        TestHelper::MouseWheelScrolled(data);
    }

    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) override {
        TestHelper::handleMouseCoordinatesNormalized(data);
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentEffect) {
            currentEffect->update(deltaTime);
            currentEffect->draw();
        }

        renderInstructions(deltaTime, instructions);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        ctx.textManager.write("Current Effect: " + effectNames[currentEffectType],
                              Fonts::Highscore, true, 0.5f, 0.0f, -0.85f);

        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        EffectsTestContext ctx;
        const EventDispatcher eventDispatcher(&ctx.eventManager);
        EffectsTestHelper testHelper(ctx);
        const std::vector<std::string> instructions = {
            "ESC: Quit",
            "0: No Effect",
            "1:Spark, 2:Fire, 3:Smoke, 4:Rain, 5:Explosion, 6:Sparkle",
            "LMB: Create Effect at Mouse Position",
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
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
