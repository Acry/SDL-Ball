// TextManager_Tests.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
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
    EventManager eventManager;

    EventDispatcher eventDispatcher(&eventManager);
    MouseManager mouseManager(&eventManager);
    KeyboardManager keyboardManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Text Test");

    TextManager textManager(&eventManager);

    eventManager.addListener(
        GameEvent::FontThemeChanged,
        [&textManager](const ThemeData &data) {
            textManager.addAnnouncement("New Font Theme", 2000, Fonts::AnnounceGood);
        },
        nullptr
    );

    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }


    TestHelper testHelper(textManager, &eventManager);
    constexpr float yPos = 0.6f;
    constexpr float offset = 0.3f;
    const std::vector<TextTest> tests = {
        {"ABC ö ghj ? 12345", Fonts::Highscore, false, 1.0f, -0.95f, yPos}, // 80
        {"ABC ö ghj ? 12345", Fonts::AnnounceGood, false, 1.0f, -0.95f, yPos - offset * 1}, // 60
        {"ABC ö ghj ? 12345", Fonts::AnnounceBad, false, 1.0f, -0.95f, yPos - offset * 1.75f}, // 60
        {"ABC ö ghj ? 12345", Fonts::IntroHighscore, false, 1.0f, -0.95f, yPos - offset * 2.5f}, // 40
        {"ABC ö ghj ? 12345", Fonts::Menu, false, 1.0f, -0.95f, yPos - offset * 3.0f}, // 30
        {"ABC ö ghj ? 12345", Fonts::MenuHighscore, false, 1.0f, -0.95f, yPos - offset * 3.5f}, // 28
        {"ABC ö ghj ? 12345", Fonts::IntroDescription, false, 1.0f, -0.95f, yPos - offset * 4.0f} // 12
    };

    const std::vector<std::string> instructions = {
        "1: Good Announcement",
        "2: Bad Announcement",
        "3: Load Test Theme via event",
        "4: Load Default Theme via event",
        "M: Toggle Mouse Coordinates",
        "S: Create Screenshot",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        running = eventDispatcher.processEvents();

        if (const Uint8 *state = SDL_GetKeyboardState(nullptr); state[SDL_SCANCODE_1]) {
            textManager.addAnnouncement("Well Done!", 2500, Fonts::AnnounceGood);
        } else if (state[SDL_SCANCODE_2]) {
            textManager.addAnnouncement("GAME OVER!", 10000, Fonts::AnnounceBad);
        } else if (state[SDL_SCANCODE_3]) {
            const ThemeData testThemeData = {.fontsTheme = {"test", "../tests/themes/test"}};
            eventManager.emit(GameEvent::FontThemeRequested, testThemeData);
        } else if (state[SDL_SCANCODE_4]) {
            const ThemeData themeData = {.fontsTheme = {"default", "../themes/default"}};
            eventManager.emit(GameEvent::FontThemeRequested, themeData);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid(0.2f);
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        // font heights
        for (int i = 0; i < static_cast<int>(Fonts::Count); i++) {
            std::string heightInfo = "Font " + std::to_string(i) + " height: " +
                                     std::to_string(textManager.getHeight(static_cast<Fonts>(i)));
            textManager.write(heightInfo, Fonts::MenuHighscore, false, 1.0f, 0.25f, 0.0 - i * 0.08f);
        }
        // render theme fonts

        textManager.write("TOP-LEFT", Fonts::IntroDescription, false, 1.0f, -1.0f, 1.0f);
        for (const auto &[text, font, centered, scale, x, y]: tests) {
            textManager.write(text, font, centered, scale, x, y);
        }

        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
