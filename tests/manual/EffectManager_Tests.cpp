// EffectManager_Tests.cpp
#include <cstdlib>
#include "DisplayManager.hpp"
#include "EffectManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimation.h"
#include "TextManager.h"
#include "Tracer.h"

int main() {
    DisplayManager display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    EventManager eventManager;
    TextureManager textureManager;
    const std::filesystem::path themePath = "../themes/default";
    textureManager.setSpriteTheme(themePath);

    EffectManager effectManager(&eventManager);
    SpriteSheetAnimation effectTexture = *textureManager.getEffectTexture(EffectTexture::Particle);
    effectManager.set(FX_VAR_TEXTURE, effectTexture);

    int tracerId = effectManager.createTracer(0.04f, 0.04f, false);
    effectManager.setTracerTexture(tracerId, *textureManager.getEffectTexture(EffectTexture::Tail));
    effectManager.setTracerActive(tracerId, false);

    float mouseX = 0.0f, mouseY = 0.0f;
    bool tracerActive = false;
    std::vector<std::string> instructions = {
        "1: Sparks",
        "2: Destroy bricks",
        "3: PowerUp",
        "4: Fading",
        "5: Partikel field",
        "6: Ball Tracer",
        "ESC: Beenden"
    };

    Uint32 lastTime = SDL_GetTicks();
    Uint32 frameStartTime;
    bool running = true;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    while (running) {
        // Zeit für Animation und Update aktualisieren
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Besseres Timing für stabilere Framerate
        const int targetFPS = 60;
        const int targetFrameTime = 1000 / targetFPS;
        frameStartTime = SDL_GetTicks(); // Startzeit des aktuellen Frames setzen
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

            if (event.type == SDL_MOUSEMOTION) {
                // Mausposition normalisieren (-1 bis 1)
                mouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) *
                         (2.0f / display.viewportW);
                mouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 *
                         (2.0f / display.viewportH);
                mouseX = std::max(-1.0f, std::min(1.0f, mouseX));
                mouseY = std::max(-1.0f, std::min(1.0f, mouseY));
            }
            if (event.type == SDL_KEYDOWN) {
                position mousePos = {mouseX, mouseY};
                EventData eventData;
                eventData.posX = mouseX;
                eventData.posY = mouseY;
                position p = {0.0f, 0.0f}; // Für Transition
                position rectSize = {0.2f, 0.2f}; // Für Partikelfeld

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;

                    case SDLK_1: // Einfacher Funkeneffekt (manuell)
                        SDL_Log("Funkeneffekt an Position (%.2f, %.2f)", mouseX, mouseY);
                        effectManager.set(FX_VAR_TYPE, FX_SPARKS);
                        effectManager.set(FX_VAR_NUM, 20);
                        effectManager.set(FX_VAR_LIFE, 400);
                        effectManager.set(FX_VAR_SPEED, 0.006f);
                        effectManager.set(FX_VAR_SIZE, 0.025f);
                        effectManager.set(FX_VAR_COLOR, 1.0f, 0.6f, 0.2f);
                        effectManager.set(FX_VAR_GRAVITY, 0.001f);
                        effectManager.spawn(mousePos);
                        break;

                    case SDLK_2: // Brick-Zerstörungs-Effekt über Event
                        SDL_Log("Brick-Zerstörungs-Effekt über Event an Position (%.2f, %.2f)", mouseX, mouseY);
                        eventManager.emit(GameEvent::BrickDestroyed, eventData);
                        break;

                    case SDLK_3: // PowerUp-Sammel-Effekt über Event
                        SDL_Log("PowerUp-Sammel-Effekt über Event an Position (%.2f, %.2f)", mouseX, mouseY);
                        eventManager.emit(GameEvent::PowerUpCollected, eventData);
                        break;

                    case SDLK_4: // Übergangseffekt
                        SDL_Log("Übergangseffekt (Fade) gestartet");
                        effectManager.set(FX_VAR_TYPE, FX_TRANSIT);
                        effectManager.set(FX_VAR_LIFE, 5000);
                        effectManager.set(FX_VAR_COLOR, 0.0f, 0.0f, 0.0f); // Schwarzer Übergang
                        effectManager.spawn(p); // Bereits deklarierte Variable verwenden
                        break;

                    case SDLK_5: // Partikelfeld-Effekt
                        SDL_Log("Partikelfeld an Position (%.2f, %.2f)", mouseX, mouseY);
                        effectManager.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
                        effectManager.set(FX_VAR_NUM, 50);
                        effectManager.set(FX_VAR_LIFE, 800);
                        effectManager.set(FX_VAR_SPEED, 0.005f);
                        effectManager.set(FX_VAR_SIZE, 0.02f);
                        effectManager.set(FX_VAR_COLOR, 0.4f, 0.8f, 1.0f);
                        effectManager.set(FX_VAR_GRAVITY, 0.0005f);
                        effectManager.set(FX_VAR_RECTANGLE, rectSize); // Bereits deklarierte Variable verwenden
                        effectManager.spawn(mousePos);
                        break;
                    case SDLK_6:
                        tracerActive = !tracerActive;
                        effectManager.setTracerActive(tracerId, tracerActive);
                        break;
                    case SDLK_p: // Ball-Paddle-Kollisionseffekt über Event
                        SDL_Log("Ball-Paddle-Kollisionseffekt über Event an Position (%.2f, %.2f)", mouseX, mouseY);
                        eventManager.emit(GameEvent::BallHitPaddle, eventData);
                        break;
                    default: ;
                }
            }
        }
        // Szene rendern
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (tracerActive) {
            effectManager.updateTracer(tracerId, mouseX, mouseY);
            // Regenbogenfarben über Zeit (HSV-zu-RGB Konvertierung)
            static float hue = 0.0f;
            hue += deltaTime * 0.5f; // Geschwindigkeit der Farbänderung
            if (hue > 1.0f) hue -= 1.0f;

            // HSV zu RGB Konvertierung
            float h = hue * 6.0f;
            float s = 1.0f; // Sättigung
            float v = 1.0f; // Helligkeit
            float c = v * s;
            float x = c * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
            float m = v - c;

            GLfloat r = 0.0f, g = 0.0f, b = 0.0f;

            if (h < 1.0f) {
                r = c;
                g = x;
                b = 0.0f;
            } else if (h < 2.0f) {
                r = x;
                g = c;
                b = 0.0f;
            } else if (h < 3.0f) {
                r = 0.0f;
                g = c;
                b = x;
            } else if (h < 4.0f) {
                r = 0.0f;
                g = x;
                b = c;
            } else if (h < 5.0f) {
                r = x;
                g = 0.0f;
                b = c;
            } else {
                r = c;
                g = 0.0f;
                b = x;
            }

            r += m;
            g += m;
            b += m;

            GLfloat colors[3] = {r, g, b};
            effectManager.setTracerColor(tracerId, false, colors);
        }

        effectManager.draw(deltaTime);

        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        float yPos = 0.9f;
        for (const auto &instruction: instructions) {
            textManager.write(instruction, Fonts::Menu, true, 0.75f, -0.0f, yPos);
            yPos -= 0.07f;
        }
        SDL_GL_SwapWindow(display.sdlWindow);
        int frameTime = SDL_GetTicks() - frameStartTime;
        if (frameTime < targetFrameTime) {
            SDL_Delay(targetFrameTime - frameTime);
        }
    }
    effectManager.removeTracer(tracerId);
    textureManager.clearTheme();
    SDL_Log("EffectManager-Tests beendet");
    return EXIT_SUCCESS;
}
