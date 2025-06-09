#include <cstdlib>

#include "BackgroundManager.h"
#include "Display.hpp"
#include "TestHelper.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "EventManager.h"
#include "LevelManager.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Error creating SDL window: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: Background Test");

    const std::filesystem::path themePath = "../themes/default";

    TextManager textManager;
    if (!textManager.setTheme(themePath)) {
        SDL_Log("Error loading font theme %s", themePath.c_str());
        return EXIT_FAILURE;
    }

    TestHelper testHelper(textManager);

    EventManager eventManager;
    TextureManager textureManager;

    if (!textureManager.setBackgroundTheme(themePath)) {
        SDL_Log("Error loading backgrounds: %s", themePath.c_str());
        return EXIT_FAILURE;
    }

    BackgroundManager backgroundManager(textureManager);
    backgroundManager.registerEvents(&eventManager);

    LevelManager levelManager(&eventManager);
    if (!levelManager.setTheme(themePath)) {
        SDL_Log("Error setting level theme %s", themePath.c_str());
    }

    size_t currentLevel = 1;
    levelManager.loadLevel(currentLevel);

    const std::vector<std::string> instructions = {
        "Rendering Background based on level",
        "M: Toggle Mouse Coordinates",
        "O: Toggle Background Overlay",
        "S: Create Screenshot",
        "T: Toggle Theme",
        "-> - next level",
        "<- - previous level",
        "Pos1 - first level",
        "End - last level",
        "ESC: Quit"
    };

    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    auto levelCount = levelManager.getLevelCount();
    bool running = true;
    GLfloat normalizedMouseX = 0.0f, normalizedMouseY = 0.0f;
    const std::filesystem::path screenshotPath = "./screenshots/";
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
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * (
                                       2.0f / display.viewportW);
                normalizedMouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 * (
                                       2.0f / display.viewportH);
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_s:
                        if (!std::filesystem::exists(screenshotPath)) {
                            std::filesystem::create_directories(screenshotPath);
                        }
                        if (display.screenshot(screenshotPath)) {
                            textManager.addAnnouncement("Screenshot saved.", 1500, Fonts::AnnounceGood);
                        } else {
                            textManager.addAnnouncement("Screenshot not created.", 1500, Fonts::AnnounceBad);
                        }
                        break;
                    case SDLK_m:
                        testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                        break;
                    case SDLK_o:
                        backgroundManager.setBackgroundOverlayEnabled(!backgroundManager.isBackgroundOverlayEnabled());
                        break;
                    case SDLK_t: {
                        static bool useDefaultTheme = false;
                        const std::filesystem::path newTheme = useDefaultTheme
                                                                   ? "../themes/default"
                                                                   : "../tests/themes/test";

                        if (!textureManager.setBackgroundTheme(newTheme)) {
                            textManager.addAnnouncement("Error changing theme", 1500, Fonts::AnnounceBad);
                        } else {
                            textManager.addAnnouncement("Theme changed", 1500, Fonts::AnnounceGood);
                            // levelManager.loadLevel(currentLevel);
                        }

                        useDefaultTheme = !useDefaultTheme;
                        break;
                    }
                    case SDLK_SPACE:
                        currentLevel++;
                        if (currentLevel > levelCount) currentLevel = 1; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_HOME:
                        currentLevel = 1; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_END:
                        currentLevel = levelCount; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_LEFT:
                        currentLevel--;
                        if (currentLevel < 1) currentLevel = levelCount; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_RIGHT:
                        currentLevel++;
                        if (currentLevel > levelCount) currentLevel = 1; {
                            levelManager.loadLevel(currentLevel);
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default: ;
                }
            }
        }
        // Aktuellen Zeitpunkt abrufen
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        // Alle 5 Sekunden Level erhöhen und Hintergrund aktualisieren
        // if (currentTime - lastTime >= 5000) {
        //     currentLevel++;
        //     if (currentLevel > 50) currentLevel = 1; // Optional: Zurücksetzen nach Level 50
        //     SDL_Log("Level erhöht auf: %d", currentLevel);
        //     lastTime = currentTime;
        // }
        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        backgroundManager.draw();
        testHelper.renderInstructions(deltaTime, instructions);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        char tempTexto[64];
        sprintf(tempTexto, "Overlay: %s", backgroundManager.isBackgroundOverlayEnabled() ? "ON" : "OFF");
        textManager.write(tempTexto, Fonts::Highscore, true, 0.5f, 0.0f, -0.4f);

        char tempText[64];
        sprintf(tempText, "Level: %lu", currentLevel);
        textManager.write(tempText, Fonts::Highscore, true, 1.0f, 0.0f, -0.5f);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
