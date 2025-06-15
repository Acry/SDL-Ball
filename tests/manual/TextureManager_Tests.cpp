#include <cstdlib>

#include "TextureManager.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"

// Test theme loading
// next:
// SpriteSheetAnimation *TextureManager::getBrickTexture(BrickTexture type)
// render all textures, except backgrounds
int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: TextureTheme Test");

    TextManager textManager;
    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
        return EXIT_FAILURE;
    }
    const EventDispatcher eventDispatcher(&eventManager);
    TestHelper testHelper(textManager, &eventManager);

    TextureManager textureManager;
    const std::filesystem::path themePath = "../themes/default";

    if (!textureManager.setSpriteTheme(themePath)) {
        SDL_Log("Fehler beim Laden der Textur: %s", themePath.c_str());
        return EXIT_FAILURE;
    }

    auto [textureProperties, animationProperties] = *textureManager.getBallTexture(BallTexture::Fireball);
    SpriteSheetAnimation spriteSheetAnimation(animationProperties);

    spriteSheetAnimation.isPlaying = true;

    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        running = eventDispatcher.processEvents();

        spriteSheetAnimation.play(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid();
        testHelper.drawCenterLines();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, textureProperties.id);

        glColor4fv(textureProperties.textureColor);

        glBegin(GL_QUADS);

        glTexCoord2f(spriteSheetAnimation.currentTexturePosition[0], spriteSheetAnimation.currentTexturePosition[1]);
        glVertex2f(-0.5f, -0.5f);

        glTexCoord2f(spriteSheetAnimation.currentTexturePosition[2], spriteSheetAnimation.currentTexturePosition[3]);
        glVertex2f(0.5f, -0.5f);

        glTexCoord2f(spriteSheetAnimation.currentTexturePosition[4], spriteSheetAnimation.currentTexturePosition[5]);
        glVertex2f(0.5f, 0.5f);

        glTexCoord2f(spriteSheetAnimation.currentTexturePosition[6], spriteSheetAnimation.currentTexturePosition[7]);
        glVertex2f(-0.5f, 0.5f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        testHelper.renderInstructions(deltaTime, instructions);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    textureManager.clearTheme();
    return EXIT_SUCCESS;
}
