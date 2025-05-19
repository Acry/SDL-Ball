#include <cstdlib>

#include "BackgroundManager.h"
#include "Display.hpp"
#include "TextureManager.h"

int globalTicksSinceLastDraw = 0;

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        // Fehlerbehandlung falls Initialisierung fehlschlägt
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SpriteSheetAnimation texture;
    const TextureManager textureManager;
    BackgroundManager backgroundManager(textureManager);
    const std::filesystem::path pathName = "../themes/default/gfx/bg/";
    Uint32 level = 1;
    Uint32 lastTime = SDL_GetTicks();
    backgroundManager.updateBgIfNeeded(level, pathName);
    bool running = true;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (running) {
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
            // Screenshot-Funktion bei Tastendruck "S"
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_s) {
                    // Screenshot erstellen und im Ordner "screenshots" speichern
                    const std::filesystem::path screenshotPath = "./screenshots/";

                    // Erstelle den Ordner, falls er nicht existiert
                    if (!std::filesystem::exists(screenshotPath)) {
                        std::filesystem::create_directories(screenshotPath);
                    }
                    if (display.screenshot(screenshotPath)) {
                        SDL_Log("Screenshot erstellt");
                    } else {
                        SDL_Log("Screenshot konnte nicht erstellt werden");
                    }
                }
            }
        }
        // Aktuellen Zeitpunkt abrufen
        Uint32 currentTime = SDL_GetTicks();

        // Alle 5 Sekunden Level erhöhen und Hintergrund aktualisieren
        if (currentTime - lastTime >= 5000) {
            level++;
            if (level > 50) level = 1; // Optional: Zurücksetzen nach Level 50
            SDL_Log("Level erhöht auf: %d", level);

            backgroundManager.updateBgIfNeeded(level, pathName);
            lastTime = currentTime;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        backgroundManager.draw();
        SDL_Delay(13);
        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
