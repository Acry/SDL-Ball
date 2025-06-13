// SpriteSheetAnimation_Tests.cpp
#include <cstdlib>

#include "DisplayManager.hpp"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: SpriteSheet Animation Test");

    TextManager textManager;
    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }

    TestHelper testHelper(textManager, &eventManager);
    texture uvTest{};
    texture spriteSheetAnimation1{};
    texture spriteSheetAnimation2{};
    texture spriteSheetAnimation3{};
    texture spriteSheetAnimation4{};
    texture spriteSheetAnimation5{};

    SpriteSheetAnimation anim1(spriteSheetAnimation1.animationProperties);
    SpriteSheetAnimation anim2(spriteSheetAnimation2.animationProperties);
    SpriteSheetAnimation anim3(spriteSheetAnimation3.animationProperties);
    SpriteSheetAnimation anim4(spriteSheetAnimation4.animationProperties);
    SpriteSheetAnimation anim5(spriteSheetAnimation5.animationProperties);

    const std::filesystem::path texturePath1 = "../tests/textures/Spritesheet-Test-1";
    const std::filesystem::path texturePath2 = "../tests/textures/Spritesheet-Test-2";
    const std::filesystem::path texturePath3 = "../tests/textures/Spritesheet-Test-3";
    const std::filesystem::path texturePath4 = "../tests/textures/Spritesheet-Test-4";
    const std::filesystem::path texturePath5 = "../themes/default/gfx/powerup/gun";

    TextureManager textureManager;

    // UV test grid bg
    if (!textureManager.load("../tests/textures/UV-Test-Grid.png", uvTest)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath1.c_str());
        return EXIT_FAILURE;
    }

    if (!textureManager.loadTextureWithProperties(texturePath1, spriteSheetAnimation1)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath1.c_str());
        return EXIT_FAILURE;
    }

    if (!textureManager.loadTextureWithProperties(texturePath2, spriteSheetAnimation2)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath2.c_str());
        return EXIT_FAILURE;
    }

    if (!textureManager.loadTextureWithProperties(texturePath3, spriteSheetAnimation3)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath3.c_str());
        return EXIT_FAILURE;
    }

    if (!textureManager.loadTextureWithProperties(texturePath4, spriteSheetAnimation4)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath4.c_str());
        return EXIT_FAILURE;
    }

    if (!textureManager.loadTextureWithProperties(texturePath5, spriteSheetAnimation5)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath5.c_str());
        return EXIT_FAILURE;
    }

    anim1 = SpriteSheetAnimation(spriteSheetAnimation1.animationProperties);
    anim2 = SpriteSheetAnimation(spriteSheetAnimation2.animationProperties);
    anim3 = SpriteSheetAnimation(spriteSheetAnimation3.animationProperties);
    anim4 = SpriteSheetAnimation(spriteSheetAnimation4.animationProperties);
    anim5 = SpriteSheetAnimation(spriteSheetAnimation5.animationProperties);

    std::vector<std::string> instructions = {
        "1: bidir = 0",
        "2: bidir = 1",
        "3: bidir = 0, playOnce = 1",
        "4: bidir = 1, playOnce = 1",
        "5: Gun Powerup",
        "SPACE: Toggle Animation",
        "ESC: Quit"
    };

    anim1.isPlaying = false;
    anim2.isPlaying = false;
    anim3.isPlaying = false;
    anim4.isPlaying = false;
    anim5.isPlaying = false;

    int currentTexture = 1;

    SpriteSheetAnimation *currentAnimation = &anim1;
    texture *currentTextureObj = &spriteSheetAnimation1;

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running) {
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
                    displayManager.resize(event.window.data1, event.window.data2);
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_SPACE:
                            currentAnimation->isPlaying = !currentAnimation->isPlaying;
                            break;
                        case SDLK_1:
                            currentTexture = 1;
                            currentAnimation = &anim1;
                            currentTextureObj = &spriteSheetAnimation1;
                            break;
                        case SDLK_2:
                            currentTexture = 2;
                            currentAnimation = &anim2;
                            currentTextureObj = &spriteSheetAnimation2;
                            break;
                        case SDLK_3:
                            currentTexture = 3;
                            currentAnimation = &anim3;
                            currentTextureObj = &spriteSheetAnimation3;
                            break;
                        case SDLK_4:
                            currentTexture = 4;
                            currentAnimation = &anim4;
                            currentTextureObj = &spriteSheetAnimation4;
                            break;
                        case SDLK_5:
                            currentTexture = 5;
                            currentAnimation = &anim5;
                            currentTextureObj = &spriteSheetAnimation5;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        if (currentAnimation->isPlaying) {
            currentAnimation->play(deltaTime);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Hier die Textur zeichnen
        glEnable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Zeichnen der UV-Test-Textur im Hintergrund
        glColor4f(0.7f, 0.7f, 0.7f, 0.5f);
        glBindTexture(GL_TEXTURE_2D, uvTest.textureProperties.id);
        glBegin(GL_QUADS);

        // Bottom-left corner
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);

        // Bottom-right corner
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);

        // Top-right corner
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);

        // Top-left corner
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);

        glEnd();

        glBindTexture(GL_TEXTURE_2D, currentTextureObj->textureProperties.id);

        // Mit den Texturfarben färben
        glColor4fv(currentTextureObj->textureProperties.textureColor);

        // Vergrößert in der Mitte zeichnen
        glBegin(GL_QUADS);

        // Bottom-left corner
        glTexCoord2f(currentAnimation->currentTexturePosition[0], currentAnimation->currentTexturePosition[1]);
        glVertex2f(-0.5f, -0.5f);

        // Bottom-right corner
        glTexCoord2f(currentAnimation->currentTexturePosition[2], currentAnimation->currentTexturePosition[3]);
        glVertex2f(0.5f, -0.5f);

        // Top-right corner
        glTexCoord2f(currentAnimation->currentTexturePosition[4], currentAnimation->currentTexturePosition[5]);
        glVertex2f(0.5f, 0.5f);

        // Top-left corner
        glTexCoord2f(currentAnimation->currentTexturePosition[6], currentAnimation->currentTexturePosition[7]);
        glVertex2f(-0.5f, 0.5f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);


        testHelper.renderInstructions(deltaTime, instructions);

        std::string textureName;
        switch (currentTexture) {
            case 1: textureName = "bidir = 0";
                break;
            case 2: textureName = "bidir = 1";
                break;
            case 3: textureName = "bidir = 0, playOnce = 1";
                break;
            case 4: textureName = "bidir = 1, playOnce = 1";
                break;
            case 5: textureName = "Gun Powerup";
                break;
            default: textureName = "Unknown";
                break;
        }

        std::string statusText = "Current Texture: " + textureName;
        textManager.write(statusText, Fonts::Menu, true, 0.8f, 0.0f, -0.7f);

        std::string animationStatus = currentAnimation->isPlaying ? "Animation: running" : "Animation: paused";
        textManager.write(animationStatus, Fonts::Menu, true, 0.7f, 0.0f, -0.8f);

        textManager.write("A1", Fonts::IntroHighscore, false, 1.0f, -0.95f, -0.80f);

        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }

    glDeleteTextures(1, &uvTest.textureProperties.id);
    glDeleteTextures(1, &spriteSheetAnimation1.textureProperties.id);
    glDeleteTextures(1, &spriteSheetAnimation2.textureProperties.id);
    glDeleteTextures(1, &spriteSheetAnimation3.textureProperties.id);
    glDeleteTextures(1, &spriteSheetAnimation4.textureProperties.id);
    glDeleteTextures(1, &spriteSheetAnimation5.textureProperties.id);

    return EXIT_SUCCESS;
}
