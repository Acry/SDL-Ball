// TextManager_Tests.cpp
#include <cstdlib>
#include <SDL2/SDL.h>
#include <epoxy/gl.h>
#include <string>
#include <vector>

#include "Display.hpp"
#include "TestHelper.h"
#include "TextManager.h"

struct TextTest {
    std::string text;
    Fonts font;
    bool centered;
    GLfloat scale;
    GLfloat x;
    GLfloat y;
};

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: Text Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }

    TestHelper testHelper;

    const std::vector<TextTest> tests = {
        {"FONT HIGHSCORE", Fonts::Highscore, false, 1.0f, -0.9f, 0.6f}, // 80
        {"FONT ANNOUNCE_GOOD", Fonts::AnnounceGood, false, 1.0f, -0.9f, 0.45f}, // 60
        {"FONT ANNOUNCE_BAD", Fonts::AnnounceBad, false, 1.0f, -0.9f, 0.35f}, // 60
        {"FONT INTROHIGHSCORE", Fonts::IntroHighscore, false, 1.0f, -0.9f, 0.25f}, // 40
        {"FONT MENU", Fonts::Menu, false, 1.0f, -0.9f, 0.15f}, // 30
        {"FONT MENUHIGHSCORE", Fonts::MenuHighscore, false, 1.0f, -0.9f, 0.05f}, // 28
        {"FONT INTRODESCRIPTION", Fonts::IntroDescription, false, 1.0f, -0.9f, -0.05f} // 12
    };

    const std::vector<std::string> instructions = {
        "1: Good Announcement",
        "2: Bad Announcement",
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
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        running = false;
                    else if (event.key.keysym.sym == SDLK_1) {
                        textManager.addAnnouncement("Well Done!", 1500, Fonts::AnnounceGood);
                    } else if (event.key.keysym.sym == SDLK_2) {
                        textManager.addAnnouncement("GAME OVER!", 10000, Fonts::AnnounceBad);
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        display.resize(event.window.data1, event.window.data2);
                    }
                    break;
                default:
                    break;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid(0.2f);
        testHelper.drawCenterLines();

        // Instructions
        glColor4f(0.95f, 0.45f, 0.05f, 1.0f);
        float yPos = 0.9f;
        for (const auto &instruction: instructions) {
            textManager.write(instruction, Fonts::Menu, true, 1.0f, 0.0f, yPos);
            yPos -= 0.07f;
        }

        // Schrifthöhen anzeigen
        for (int i = 0; i < static_cast<int>(Fonts::Count); i++) {
            std::string heightInfo = "Font " + std::to_string(i) + " height: " +
                                     std::to_string(textManager.getHeight(static_cast<Fonts>(i)));
            textManager.write(heightInfo, Fonts::IntroDescription, false, 1.0f, 0.5f, 0.6f - i * 0.08f);
        }

        glColor4f(1.0f, 0.98f, 0.94f, 1.0f);
        for (const auto &[text, font, centered, scale, x, y]: tests) {
            textManager.write(text, font, centered, scale, x, y);
        }


        if (textManager.getAnnouncementCount() > 0) {
            textManager.updateAnnouncements(deltaTime);
            textManager.drawAnnouncements(deltaTime);
        }
        // currently I use VSync, so this is not needed
        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
