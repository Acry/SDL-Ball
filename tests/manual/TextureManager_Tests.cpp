// TextureManager_Tests.cpp
#include <cstdlib>

#include "TextureManager.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"

// FIXME: Theme Change, Blocker: Alternative Theme (3.1) not yet implemented
class MyTestHelper final : public TestHelper {
public:
    using TestHelper::TestHelper;

    size_t currentTextureIndex = 0;
    std::vector<std::pair<std::string, texture *> > texturesToDisplay;


    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);

        if (texturesToDisplay.empty())
            return;

        switch (data.key) {
            case SDLK_LEFT:
                if (currentTextureIndex > 0)
                    currentTextureIndex--;
                else
                    currentTextureIndex = texturesToDisplay.size() - 1;
                break;
            case SDLK_RIGHT:
                currentTextureIndex = (currentTextureIndex + 1) % texturesToDisplay.size();
                break;
            case SDLK_HOME:
                currentTextureIndex = 0;
                break;
            case SDLK_END:
                currentTextureIndex = texturesToDisplay.size() - 1;
                break;
            default:
                break;
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
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: TextureTheme Test");

    TextManager textManager(&eventManager);
    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
        return EXIT_FAILURE;
    }
    const EventDispatcher eventDispatcher(&eventManager);
    MyTestHelper testHelper(textManager, &eventManager);

    TextureManager textureManager;
    const std::filesystem::path themePath = "../themes/default";

    if (!textureManager.setSpriteTheme(themePath)) {
        SDL_Log("Fehler beim Laden der Textur: %s", themePath.c_str());
        return EXIT_FAILURE;
    }

    for (const auto &[path, texPtr]: textureManager.getTextureCache()) {
        testHelper.texturesToDisplay.push_back({path, texPtr.get()});
    }

    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "-> - next texture",
        "<- - previous texture",
        "Pos1 - first texture",
        "End - last texture",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        running = eventDispatcher.processEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid();
        testHelper.drawCenterLines();

        if (!testHelper.texturesToDisplay.empty()) {
            auto &[name, currentTexture] = testHelper.texturesToDisplay[testHelper.currentTextureIndex];

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, currentTexture->textureProperties.id);

            glColor4fv(currentTexture->textureProperties.textureColor);

            glBegin(GL_QUADS);

            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-0.5f, -0.5f);

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(0.5f, -0.5f);

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(0.5f, 0.5f);

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(-0.5f, 0.5f);

            glEnd();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);

            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            std::string infoText = "Texture: " + name;
            textManager.write(infoText, Fonts::Menu, true, 1.0f, 0.0f, -0.6f);
        }
        testHelper.renderInstructions(deltaTime, instructions);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    textureManager.clearTheme();
    return EXIT_SUCCESS;
}
