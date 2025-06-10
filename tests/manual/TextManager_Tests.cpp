// TextManager_Tests.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "DisplayManager.hpp"
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
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Text Test");

    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }

    TestHelper testHelper(textManager);
    constexpr float yPos = 0.0f;
    constexpr float offset = 0.2f;
    const std::vector<TextTest> tests = {
        {"ABC ö ghj ?", Fonts::Highscore, false, 1.0f, -0.95f, yPos}, // 80
        {"ABC ö ghj ?", Fonts::AnnounceGood, false, 1.0f, -0.95f, yPos - offset * 1}, // 60
        {"ABC ö ghj ?", Fonts::AnnounceBad, false, 1.0f, -0.95f, yPos - offset * 1.75f}, // 60
        {"ABC ö ghj ?", Fonts::IntroHighscore, false, 1.0f, -0.95f, yPos - offset * 2.5f}, // 40
        {"ABC ö ghj ?", Fonts::Menu, false, 1.0f, -0.95f, yPos - offset * 3.0f}, // 30
        {"ABC ö ghj ?", Fonts::MenuHighscore, false, 1.0f, -0.95f, yPos - offset * 3.5f}, // 28
        {"ABC ö ghj ?", Fonts::IntroDescription, false, 1.0f, -0.95f, yPos - offset * 4.0f} // 12
    };

    const std::vector<std::string> instructions = {
        "1: Good Announcement",
        "2: Bad Announcement",
        "M: Toggle Mouse Coordinates",
        "S: Create Screenshot",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    GLfloat normalizedMouseX = 0.0f, normalizedMouseY = 0.0f;
    while (running) {
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
                    } else if (event.key.keysym.sym == SDLK_m) {
                        testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                    } else if (event.key.keysym.sym == SDLK_s) {
                        const std::filesystem::path screenshotPath = "./screenshots/";
                        if (!std::filesystem::exists(screenshotPath)) {
                            std::filesystem::create_directories(screenshotPath);
                        }
                        if (displayManager.screenshot(screenshotPath)) {
                            textManager.addAnnouncement("Screenshot saved.", 1500, Fonts::AnnounceGood);
                        } else {
                            textManager.addAnnouncement("Screenshot not created.", 1500, Fonts::AnnounceBad);
                        }
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        displayManager.resize(event.window.data1, event.window.data2);
                    }
                    break;
                case SDL_MOUSEMOTION: {
                    normalizedMouseX = (event.motion.x - displayManager.viewportX - displayManager.viewportW / 2.0f) * (
                                           2.0f / displayManager.viewportW);
                    normalizedMouseY = (event.motion.y - displayManager.viewportY - displayManager.viewportH / 2.0f) * -
                                       1 * (
                                           2.0f / displayManager.viewportH);
                }
                default:
                    break;
            }
        }

        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid(0.2f);
        testHelper.drawCenterLines();
        testHelper.renderInstructions(deltaTime, instructions);

        // font heights
        for (int i = 0; i < static_cast<int>(Fonts::Count); i++) {
            std::string heightInfo = "Font " + std::to_string(i) + " height: " +
                                     std::to_string(textManager.getHeight(static_cast<Fonts>(i)));
            textManager.write(heightInfo, Fonts::IntroDescription, false, 2.0f, 0.25f, yPos - i * 0.08f);
        }
        // render theme fonts
        glColor4f(1.0f, 0.98f, 0.94f, 1.0f);
        textManager.write("TOP-LEFT", Fonts::IntroDescription, false, 1.0f, -1.0f, 1.0f);
        for (const auto &[text, font, centered, scale, x, y]: tests) {
            textManager.write(text, font, centered, scale, x, y);
        }

        if (textManager.getAnnouncementCount() > 0) {
            textManager.updateAnnouncements(deltaTime);
            textManager.drawAnnouncements(deltaTime);
        }
        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
