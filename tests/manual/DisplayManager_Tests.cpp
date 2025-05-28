#include <cstdlib>

#include "Display.hpp"
#include "colors.h"
#include "Grid.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    Grid grid;
    // Oder mit benutzerdefiniertem Spacing: Grid grid(0.2f);

    glClearColor(GL_DARK_BLUE);
    bool running = true;

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        grid.draw();
        SDL_Delay(13);
        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
