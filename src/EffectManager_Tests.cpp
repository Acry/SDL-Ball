#include <cstdlib>
#include "Display.hpp"
#include "EffectManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimation.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    EventManager eventManager;
    const TextureManager textureManager;
    EffectManager effectManager(&eventManager);

    // Effekt-Textur laden
    SpriteSheetAnimation effectTexture;
    const std::filesystem::path texturePath = "../themes/default/gfx/effects/particle.png";
    if (!textureManager.load(texturePath, effectTexture)) {
        SDL_Log("Fehler beim Laden der Effekt-Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }
    // Textur für den EffectManager setzen
    effectManager.set(FX_VAR_TEXTURE, effectTexture);

    float mouseX = 0.0f, mouseY = 0.0f;

    SDL_Log("EffectManager-Tests gestartet");
    SDL_Log("Tastenbelegungen:");
    SDL_Log("1: Funkeneffekt erzeugen");
    SDL_Log("2: Brickzerstörungs-Effekt simulieren");
    SDL_Log("3: PowerUp-Sammel-Effekt simulieren");
    SDL_Log("4: Übergangseffekt (Transition)");
    SDL_Log("5: Partikelfeld-Effekt");
    SDL_Log("ESC: Beenden");

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

        // Alle aktiven Effekte zeichnen
        effectManager.draw(deltaTime);

        // Position des Mauszeigers anzeigen (einfaches Quadrat)
        glDisable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
        glBegin(GL_QUADS);
        const float cursorSize = 0.02f;
        glVertex3f(mouseX - cursorSize, mouseY + cursorSize, 0.0);
        glVertex3f(mouseX + cursorSize, mouseY + cursorSize, 0.0);
        glVertex3f(mouseX + cursorSize, mouseY - cursorSize, 0.0);
        glVertex3f(mouseX - cursorSize, mouseY - cursorSize, 0.0);
        glEnd();
        SDL_GL_SwapWindow(display.sdlWindow);
        int frameTime = SDL_GetTicks() - frameStartTime;
        if (frameTime < targetFrameTime) {
            SDL_Delay(targetFrameTime - frameTime);
        }
    }

    SDL_Log("EffectManager-Tests beendet");
    return EXIT_SUCCESS;
}
