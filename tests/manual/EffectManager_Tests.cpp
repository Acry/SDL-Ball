// EffectManager_Tests.cpp
#include <cstdlib>
#include "DisplayManager.hpp"
#include "EffectManager.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimation.h"
#include "TestHelper.h"
#include "TextManager.h"

class EffectsTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;


    EffectsTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Ball Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
    }
};

class EffectsTestHelper final : public TestHelper {
    EffectsTestContext &ctx;

public:
    explicit EffectsTestHelper(EffectsTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_w:
                break;
            case SDLK_DELETE:
                break;
            case SDLK_g:
                break;
            case SDLK_k:
                break;
            case SDLK_u:
                break;
            case SDLK_l:
            default:
                break;
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        EffectsTestContext ctx;
        const EffectsTestHelper testHelper(ctx);
        const EventDispatcher eventDispatcher(&ctx.eventManager);

        EffectManager effectManager(&ctx.eventManager);
        // SpriteSheetAnimation effectTexture = *textureManager.getEffectTexture(EffectTexture::Particle);
        // effectManager.set(FX_VAR_TEXTURE, effectTexture);
        //
        // int tracerId = effectManager.createTracer(0.04f, 0.04f, false);
        // effectManager.setTracerTexture(tracerId, *textureManager.getEffectTexture(EffectTexture::Tail));
        // effectManager.setTracerActive(tracerId, false);

        // bool tracerActive = false;

        const std::vector<std::string> instructions = {
            "1: Sparks",
            "2: Destroy bricks",
            "3: PowerUp",
            "4: Fading",
            "5: Partikel field",
            "6: Ball Tracer",
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
        //if (event.type == SDL_KEYDOWN) {
        //    position mousePos = {mouseX, mouseY};
        //    EventData eventData;
        //    eventData.posX = mouseX;
        //    eventData.posY = mouseY;
        //    position p = {0.0f, 0.0f}; // Für Transition
        //    position rectSize = {0.2f, 0.2f}; // Für Partikelfeld
        //
        //    switch (event.key.keysym.sym) {
        //        case SDLK_ESCAPE:
        //            running = false;
        //            break;
        //
        //        case SDLK_1: // Einfacher Funkeneffekt (manuell)
        //            SDL_Log("Funkeneffekt an Position (%.2f, %.2f)", mouseX, mouseY);
        //            effectManager.set(FX_VAR_TYPE, FX_SPARKS);
        //            effectManager.set(FX_VAR_NUM, 20);
        //            effectManager.set(FX_VAR_LIFE, 400);
        //            effectManager.set(FX_VAR_SPEED, 0.006f);
        //            effectManager.set(FX_VAR_SIZE, 0.025f);
        //            effectManager.set(FX_VAR_COLOR, 1.0f, 0.6f, 0.2f);
        //            effectManager.set(FX_VAR_GRAVITY, 0.001f);
        //            effectManager.spawn(mousePos);
        //            break;
        //
        //        case SDLK_2: // Brick-Zerstörungs-Effekt über Event
        //            SDL_Log("Brick-Zerstörungs-Effekt über Event an Position (%.2f, %.2f)", mouseX, mouseY);
        //            eventManager.emit(GameEvent::BrickDestroyed, eventData);
        //            break;
        //
        //        case SDLK_3: // PowerUp-Sammel-Effekt über Event
        //            SDL_Log("PowerUp-Sammel-Effekt über Event an Position (%.2f, %.2f)", mouseX, mouseY);
        //            eventManager.emit(GameEvent::PowerUpCollected, eventData);
        //            break;
        //
        //        case SDLK_4: // Übergangseffekt
        //            SDL_Log("Übergangseffekt (Fade) gestartet");
        //            effectManager.set(FX_VAR_TYPE, FX_TRANSIT);
        //            effectManager.set(FX_VAR_LIFE, 5000);
        //            effectManager.set(FX_VAR_COLOR, 0.0f, 0.0f, 0.0f); // Schwarzer Übergang
        //            effectManager.spawn(p); // Bereits deklarierte Variable verwenden
        //            break;
        //
        //        case SDLK_5: // Partikelfeld-Effekt
        //            SDL_Log("Partikelfeld an Position (%.2f, %.2f)", mouseX, mouseY);
        //            effectManager.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
        //            effectManager.set(FX_VAR_NUM, 50);
        //            effectManager.set(FX_VAR_LIFE, 800);
        //            effectManager.set(FX_VAR_SPEED, 0.005f);
        //            effectManager.set(FX_VAR_SIZE, 0.02f);
        //            effectManager.set(FX_VAR_COLOR, 0.4f, 0.8f, 1.0f);
        //            effectManager.set(FX_VAR_GRAVITY, 0.0005f);
        //            effectManager.set(FX_VAR_RECTANGLE, rectSize); // Bereits deklarierte Variable verwenden
        //            effectManager.spawn(mousePos);
        //            break;
        //        case SDLK_6:
        //            tracerActive = !tracerActive;
        //            effectManager.setTracerActive(tracerId, tracerActive);
        //            break;
        //        case SDLK_p: // Ball-Paddle-Kollisionseffekt über Event
        //            SDL_Log("Ball-Paddle-Kollisionseffekt über Event an Position (%.2f, %.2f)", mouseX, mouseY);
        //            eventManager.emit(GameEvent::BallHitPaddle, eventData);
        //            break;
        //        default: ;
        //    }
        //}

        // Szene rendern

        //if (tracerActive) {
        //    effectManager.updateTracer(tracerId, mouseX, mouseY);
        //    // Regenbogenfarben über Zeit (HSV-zu-RGB Konvertierung)
        //    static float hue = 0.0f;
        //    hue += deltaTime * 0.5f; // Geschwindigkeit der Farbänderung
        //    if (hue > 1.0f) hue -= 1.0f;
        //
        //    // HSV zu RGB Konvertierung
        //    float h = hue * 6.0f;
        //    float s = 1.0f; // Sättigung
        //    float v = 1.0f; // Helligkeit
        //    float c = v * s;
        //    float x = c * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
        //    float m = v - c;
        //
        //    GLfloat r = 0.0f, g = 0.0f, b = 0.0f;
        //
        //    if (h < 1.0f) {
        //        r = c;
        //        g = x;
        //        b = 0.0f;
        //    } else if (h < 2.0f) {
        //        r = x;
        //        g = c;
        //        b = 0.0f;
        //    } else if (h < 3.0f) {
        //        r = 0.0f;
        //        g = c;
        //        b = x;
        //    } else if (h < 4.0f) {
        //        r = 0.0f;
        //        g = x;
        //        b = c;
        //    } else if (h < 5.0f) {
        //        r = x;
        //        g = 0.0f;
        //        b = c;
        //    } else {
        //        r = c;
        //        g = 0.0f;
        //        b = x;
        //    }
        //
        //    r += m;
        //    g += m;
        //    b += m;
        //
        //    GLfloat colors[3] = {r, g, b};
        //    effectManager.setTracerColor(tracerId, false, colors);
        //}


        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
