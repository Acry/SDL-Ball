// BackgroundManager_Tests.cpp
#include <cstdlib>

#include "BackgroundManager.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "TestHelper.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "EventManager.h"
#include "LevelManager.h"
#include "MouseManager.h"

LevelManager *levelManager = nullptr;
BackgroundManager *backgroundManager = nullptr;
TextureManager *textureManager = nullptr;
TextManager *textManager = nullptr;
size_t currentLevel = 1;
size_t levelCount = 0;

class MyTestHelper final : public TestHelper {
public:
    using TestHelper::TestHelper;

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data); // Basisverhalten
        switch (data.key) {
            case SDLK_o:
                backgroundManager->setBackgroundOverlayEnabled(!backgroundManager->isBackgroundOverlayEnabled());
                break;
            case SDLK_t: {
                static bool useDefaultTheme = false;
                const std::filesystem::path newTheme = useDefaultTheme
                                                           ? "../themes/default"
                                                           : "../tests/themes/test";

                if (!textureManager->setBackgroundTheme(newTheme)) {
                    textManager->addAnnouncement("Error changing theme", 1500, Fonts::AnnounceBad);
                } else {
                    textManager->addAnnouncement("Theme changed", 1500, Fonts::AnnounceGood);
                    backgroundManager->updateBgIfNeeded(currentLevel);
                }
                useDefaultTheme = !useDefaultTheme;
                break;
            }
            case SDLK_SPACE:
                currentLevel++;
                if (currentLevel > levelCount) currentLevel = 1; {
                    levelManager->loadLevel(currentLevel);
                }
                break;
            case SDLK_HOME:
                currentLevel = 1; {
                    levelManager->loadLevel(currentLevel);
                }
                break;
            case SDLK_END:
                currentLevel = levelCount; {
                    levelManager->loadLevel(currentLevel);
                }
                break;
            case SDLK_LEFT:
                currentLevel--;
                if (currentLevel < 1) currentLevel = levelCount; {
                    levelManager->loadLevel(currentLevel);
                }
                break;
            case SDLK_RIGHT:
                currentLevel++;
                if (currentLevel > levelCount) currentLevel = 1; {
                    levelManager->loadLevel(currentLevel);
                }
                break;
            default: ;
        }
    }
};

int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Background Test");

    const std::filesystem::path themePath = "../themes/default";

    textManager = new TextManager(&eventManager);
    if (!textManager->setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    MyTestHelper testHelper(*textManager, &eventManager);
    textureManager = new TextureManager();

    if (!textureManager->setBackgroundTheme(themePath)) {
        SDL_Log("Error loading backgrounds: %s", themePath.c_str());
        return EXIT_FAILURE;
    }

    backgroundManager = new BackgroundManager(*textureManager);
    backgroundManager->registerEvents(&eventManager);

    levelManager = new LevelManager(&eventManager);
    if (!levelManager->setTheme(themePath)) {
        SDL_Log("Error setting level theme %s", themePath.c_str());
    }

    levelManager->loadLevel(currentLevel);

    EventDispatcher eventDispatcher(&eventManager);

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

    levelCount = levelManager->getLevelCount();
    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        running = eventDispatcher.processEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        backgroundManager->draw();
        testHelper.renderInstructions(deltaTime, instructions);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        char tempTexto[64];
        sprintf(tempTexto, "Overlay: %s", backgroundManager->isBackgroundOverlayEnabled() ? "ON" : "OFF");
        textManager->write(tempTexto, Fonts::Highscore, true, 0.5f, 0.0f, -0.4f);

        char tempText[64];
        sprintf(tempText, "Level: %lu", currentLevel);
        textManager->write(tempText, Fonts::Highscore, true, 1.0f, 0.0f, -0.5f);
        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
