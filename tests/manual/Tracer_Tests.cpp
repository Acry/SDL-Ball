// Tracer_Tests.cpp
#include <cstdlib>
#include <vector>
#include <cmath>
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "MovingObject.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "Tracer.h"
#include "SparkleEffect.h"

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

class TracerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;

    TracerTestContext()
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
    }
};

class TracerTestHelper final : public TestHelper {
    TracerTestContext &ctx;
    Tracer *tracer;
    SparkleEffect *sparkleEffect;

public:
    explicit TracerTestHelper(TracerTestContext &context, Tracer *tracerPtr, SparkleEffect *sparkleEffectPtr)
        : TestHelper(context.textManager, &context.eventManager), ctx(context), tracer(tracerPtr),
          sparkleEffect(sparkleEffectPtr) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_w:
                break;
            case SDLK_DELETE:
                break;
            case SDLK_e: // Tracer vergrößern
                if (sparkleEffect) {
                    sparkleEffect->init({0.0f, 0.0f}); // Initialisiere den Effekt an der Mausposition
                }
                break;
            case SDLK_g: // Tracer vergrößern
                if (tracer) {
                    tracer->setSize(1.2f); // Vergrößern um 20%
                }
                break;
            case SDLK_k: // Tracer verkleinern
                if (tracer) {
                    tracer->setSize(0.8f); // Verkleinern um 20%
                }
                break;
            case SDLK_c: // Farbe des SparkleEffect ändern (C-Taste)
                if (sparkleEffect) {
                    // Zwischen verschiedenen Farben wechseln
                    static int colorIndex = 0;
                    colorIndex = (colorIndex + 1) % 16;
                    sparkleEffect->setColor(Color{
                        colors[colorIndex][0],
                        colors[colorIndex][1],
                        colors[colorIndex][2],
                        colors[colorIndex][3]
                    });
                }
                break;
            case SDLK_l:
            case SDLK_r: // R-Taste zum Ein-/Ausschalten des Regenbogeneffekts
                if (tracer) {
                    tracer->enableRainbow(!tracer->isRainbowEnabled());
                }
                break;
            default:
                break;
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
        }
    }

    void MouseWheelScrolled(const MouseEventData &data) override {
        TestHelper::MouseWheelScrolled(data);
        if (tracer) {
            if (tracer->isRainbowEnabled())
                tracer->enableRainbow(false);
            if (data.wheelY > 0) {
                // Nach oben scrollen für explosiven Modus (orange)
                constexpr GLfloat explosiveColors[3] = {1.0f, 0.6f, 0.0f}; // Orange (explosiv)
                tracer->colorRotate(true, explosiveColors);
            } else {
                // Nach unten scrollen für Farbrotation durch das colors-Array
                static int colorIndex = 0;

                // Zum nächsten Farbindex wechseln
                colorIndex = (colorIndex + 1) % 16;

                // Farbe aus dem colors-Array auswählen
                const GLfloat nextColors[3] = {
                    colors[colorIndex][0],
                    colors[colorIndex][1],
                    colors[colorIndex][2]
                };

                tracer->colorRotate(false, nextColors);
            }
        }
    }

    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) override {
        TestHelper::handleMouseCoordinatesNormalized(data);
        if (tracer) {
            tracer->updatePosition(data.x, data.y);
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();
        if (tracer) {
            tracer->update(deltaTime);
            tracer->draw();
        }

        if (sparkleEffect->isActive()) {
            sparkleEffect->update(deltaTime);
            sparkleEffect->draw();
        }
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        TracerTestContext ctx;
        const EventDispatcher eventDispatcher(&ctx.eventManager);
        const auto tracer = std::make_unique<Tracer>(*ctx.textureManager->getEffectTexture(EffectTexture::Tail));

        tracer->setSize(0.1f);

        const auto sparkle = std::make_unique<SparkleEffect>();
        sparkle->setTexture(ctx.textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);
        sparkle->setColor(Color{GL_DEEP_ORANGE});
        sparkle->setSize(0.05f);
        sparkle->setLife(5000000);
        sparkle->setParticleCount(500);
        sparkle->init({0.0f, 0.0f});
        const TracerTestHelper testHelper(ctx, tracer.get(), sparkle.get());
        const std::vector<std::string> instructions = {
            "ESC: Quit",
            "C: Change SparkleEffect Color",
            "G: Grow Tracer",
            "K: Shrink Tracer",
            "M: Toggle Mouse Coordinates",
            "E: Effect",
            "Mouse Wheel Down: Change Color",
            "Mouse Wheel Up: Explosive Color",
            "R: Toggle Rainbow Effect",
            "S: Screenshot",
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
