#include <cstdlib>
#include "Paddle.h"
#include "TextureManager.h"
#include "Display.hpp"

// Globale Variablen, die im Original verwendet werden
int globalTicksSinceLastDraw = 0;
GLfloat normalizedMouseX, normalizedMouseY;

int main() {
    // Display initialisieren
    Display display(0, 1280, 720, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }

    // TextureManager erstellen
    const TextureManager textureManager;

    // Paddle erstellen und initialisieren
    Paddle paddle;

    // Basis-Textur für das Paddle laden
    const std::filesystem::path texturePath = "../themes/default/gfx/paddle/base.png";
    const std::filesystem::path propsPath = "../themes/default/gfx/paddle/base.txt";

    if (!textureManager.load(texturePath, paddle.texture)) {
        SDL_Log("Fehler beim Laden der Paddle-Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTexProps(propsPath, paddle.texture);

    // Zusätzliche Texturen für die Layer (Glue und Gun)
    paddle.layerTex = new SpriteSheetAnimation[2];

    // Glue-Layer laden
    const std::filesystem::path glueTexPath = "../themes/default/gfx/paddle/glue.png";
    const std::filesystem::path gluePropsPath = "../themes/default/gfx/paddle/glue.txt";
    if (!textureManager.load(glueTexPath, paddle.layerTex[0])) {
        SDL_Log("Fehler beim Laden der Glue-Textur");
    } else {
        textureManager.readTexProps(gluePropsPath, paddle.layerTex[0]);
    }

    // Gun-Layer laden
    const std::filesystem::path gunTexPath = "../themes/default/gfx/powerup/gun.png";
    const std::filesystem::path gunPropsPath = "../themes/default/gfx/powerup/gun.txt";
    if (!textureManager.load(gunTexPath, paddle.layerTex[1])) {
        SDL_Log("Fehler beim Laden der Gun-Textur");
    } else {
        textureManager.readTexProps(gunPropsPath, paddle.layerTex[1]);
    }
    SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // Main Loop
    Uint32 lastTime = SDL_GetTicks();
    bool running = true;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (running) {
        // Zeit für Animation und Update aktualisieren
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        globalTicksSinceLastDraw = currentTime - lastTime;
        lastTime = currentTime;
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
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                float moveStep = 1.5f * deltaTime; // Geschwindigkeitsanpassung
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        paddle.moveTo(paddle.pos_x - moveStep, deltaTime);
                        break;
                    case SDLK_RIGHT:
                        paddle.moveTo(paddle.pos_x + moveStep, deltaTime);
                        break;
                    case SDLK_g:  // Paddle wachsen lassen
                        paddle.grow(paddle.getWidth() * 1.5f);
                        break;
                    case SDLK_s:  // Paddle verkleinern
                        paddle.grow(paddle.getWidth() * 0.7f);
                        break;
                    case SDLK_1:  // Glue aktivieren/deaktivieren
                        paddle.setGlueLayer(!paddle.hasGlueLayer);
                        break;
                    case SDLK_2:  // Gun aktivieren/deaktivieren
                        paddle.setGunLayer(!paddle.hasGunLayer);
                        break;
                    default: ;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - display.currentW / 2.0f);
                normalizedMouseY = (event.motion.y - display.currentH / 2.0f) * -1;
                normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * (
                                       2.0f / display.viewportW);
                normalizedMouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 * (
                                       2.0f / display.viewportH);
                normalizedMouseX = std::max(-1.0f, std::min(1.0f, normalizedMouseX));
                normalizedMouseY = std::max(-1.0f, std::min(1.0f, normalizedMouseY));
                paddle.moveTo(normalizedMouseX, deltaTime);
            }
        }
        paddle.update(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        paddle.draw();

        SDL_GL_SwapWindow(display.sdlWindow);
        SDL_Delay(16); // ~60fps
    }

    // Aufräumen
    delete[] paddle.layerTex;

    return EXIT_SUCCESS;
}
