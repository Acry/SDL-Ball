#include <cstdlib>
#include <vector>

#include "Display.hpp"
#include "colors.h"
#include "TestHelper.h"
#include "TextManager.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: Display Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Error loading font theme");
        return EXIT_FAILURE;
    }
    TestHelper testHelper;

    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (running) {
        // Zeit seit dem letzten Frame berechnen
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
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
                        textManager.addAnnouncement("Screenshot saved.", 1500, Fonts::AnnounceGood);
                    } else {
                        textManager.addAnnouncement("Screenshot not created.", 1500, Fonts::AnnounceBad);
                    }
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        testHelper.drawGrid();
        testHelper.drawCenterLines();

        // Instructions
        glColor4f(GL_ORANGE);
        float yPos = 0.9f;
        constexpr auto currentFont = Fonts::Menu;
        const auto height = textManager.getHeight(currentFont);
        const auto offest = height;
        for (const auto &instruction: instructions) {
            textManager.write(instruction, currentFont, true, 1.0f, 0.0f, yPos);
            yPos -= height + offest;
        }
        if (textManager.getAnnouncementCount() > 0) {
            textManager.updateAnnouncements(deltaTime);
            textManager.drawAnnouncements(deltaTime);
        }

        SDL_Delay(13);
        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
