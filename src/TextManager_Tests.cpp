// TextManager_Tests.cpp
#include <cstdlib>
#include <SDL2/SDL.h>
#include <epoxy/gl.h>
#include <string>
#include <vector>

#include "Display.hpp"
#include "TextManager.h"

struct TextTest {
    std::string text;
    int font;
    bool centered;
    GLfloat scale;
    GLfloat x;
    GLfloat y;
};

bool initGL() {
    // OpenGL-Einstellungen für 2D-Text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);

    // Orthogonale Projektion für 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    return true;
}

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }

    // Hier wird direkt eine Instanz des TextManager erstellt
    // Der Pfad muss zum tatsächlichen Speicherort der fonts.txt zeigen
    TextManager textManager;
    if (!textManager.setFontTheme("../themes/default/font/fonts.txt")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    // Text-Tests vorbereiten
    std::vector<TextTest> tests = {
        {"FONT MENU (zentriert)", FONT_MENU, true, 1.0f, 0.0f, 0.92f}, // +
        {"FONT ANNOUNCE_GOOD", FONT_ANNOUNCE_GOOD, true, 1.0f, 0.0f, 0.75f}, // +
        {"FONT ANNOUNCE_BAD", FONT_ANNOUNCE_BAD, true, 1.0f, 0.0f, 0.6f}, // +
        {"FONT HIGHSCORE", FONT_HIGHSCORE, false, 1.0f, -0.9f, 0.4f},
        {"FONT MENUHIGHSCORE", FONT_MENUHIGHSCORE, false, 1.0f, -0.9f, 0.2f}, // +
        {"FONT INTROHIGHSCORE", FONT_INTROHIGHSCORE, false, 1.0f, -0.9f, 0.0f}, // +
        {"FONT INTRODESCRIPTION", FONT_INTRODESCRIPTION, false, 1.0f, -0.9f, -0.2f} // +
    };

    glClearColor(0.05f, 0.06f, 0.2f, 1.0f);
    // Main Loop
    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running) {
        // Zeit seit dem letzten Frame berechnen
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
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
                        // Taste 1: Gute Nachricht
                        textManager.addAnnouncement("Well Done!", 1500, FONT_ANNOUNCE_GOOD);
                    }
                    else if (event.key.keysym.sym == SDLK_2) {
                        // Taste 2: Schlechte Nachricht
                        textManager.addAnnouncement("GAME OVER!", 10000, FONT_ANNOUNCE_BAD);
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

        glClear(GL_COLOR_BUFFER_BIT);

        // Alle Tests rendern
        glColor4f(1.0f, 0.98f, 0.94f, 1.0f);
        for (const auto& test : tests) {
            textManager.write(test.text, test.font, test.centered, test.scale, test.x, test.y);
        }

        // Ankündigungen aktualisieren und zeichnen
        textManager.updateAnnouncements(deltaTime);
        textManager.drawAnnouncements();

        // Zusätzliche Info
        textManager.write("ESC to quit", FONT_MENU, true, 1.0f, 0.0f, -0.95f);

        // Schrifthöhen anzeigen
        glColor4f(0.95f, 0.45f, 0.05f, 1.0f);
        for (int i = 0; i < FONT_NUM; i++) {
            std::string heightInfo = "Font " + std::to_string(i) + " height: " +
                                    std::to_string(textManager.getHeight(i));
            textManager.write(heightInfo, FONT_INTRODESCRIPTION, false, 1.0f, 0.5f, 0.0f - i * 0.08f);
        }

        SDL_GL_SwapWindow(display.sdlWindow);
        SDL_Delay(16); // ~60fps
    }
    return EXIT_SUCCESS;
}
