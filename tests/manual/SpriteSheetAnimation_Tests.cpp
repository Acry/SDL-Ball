// SpriteSheetAnimation_Tests.cpp
#include <cstdlib>

#include "TextureManager.h"
#include "DisplayManager.hpp"
#include "TextManager.h"

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: SpriteSheet Animation Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    texture uvTest{};
    texture spriteSheetAnimation1{};
    texture spriteSheetAnimation2{};
    texture spriteSheetAnimation3{};
    texture spriteSheetAnimation4{};
    texture spriteSheetAnimation5{};

    SpriteSheetAnimation anim1(uvTest.animationProperties);
    SpriteSheetAnimation anim2(spriteSheetAnimation1.animationProperties);
    SpriteSheetAnimation anim3(spriteSheetAnimation2.animationProperties);
    SpriteSheetAnimation anim4(spriteSheetAnimation3.animationProperties);
    SpriteSheetAnimation anim5(spriteSheetAnimation4.animationProperties);
    SpriteSheetAnimation anim6(spriteSheetAnimation5.animationProperties);

    const std::filesystem::path texturePath1 = "../themes/default/gfx/ball/fireball";
    const std::filesystem::path texturePath2 = "../themes/default/gfx/brick/doom";
    const std::filesystem::path texturePath3 = "../themes/default/gfx/brick/explosive";
    const std::filesystem::path texturePath4 = "../themes/default/gfx/powerup/aim";
    const std::filesystem::path texturePath5 = "../themes/default/gfx/powerup/gun";

    // TextureManager erstellen
    TextureManager textureManager;

    // UV test bg
    if (!textureManager.load("../tests/textures/UV-Test-Grid.png", uvTest)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath1.c_str());
        return EXIT_FAILURE;
    }

    // Textur 1 laden
    if (!textureManager.loadTextureWithProperties(texturePath1, spriteSheetAnimation1)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath1.c_str());
        return EXIT_FAILURE;
    }

    // Textur 2 laden
    if (!textureManager.loadTextureWithProperties(texturePath2, spriteSheetAnimation2)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath2.c_str());
        return EXIT_FAILURE;
    }

    // Textur 3 laden
    if (!textureManager.loadTextureWithProperties(texturePath3, spriteSheetAnimation3)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath3.c_str());
        return EXIT_FAILURE;
    }

    // Textur 4 laden
    if (!textureManager.loadTextureWithProperties(texturePath4, spriteSheetAnimation4)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath4.c_str());
        return EXIT_FAILURE;
    }

    // Textur 5 laden
    if (!textureManager.loadTextureWithProperties(texturePath5, spriteSheetAnimation5)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath5.c_str());
        return EXIT_FAILURE;
    }

    anim2 = SpriteSheetAnimation(spriteSheetAnimation1.animationProperties);
    anim3 = SpriteSheetAnimation(spriteSheetAnimation2.animationProperties);
    anim4 = SpriteSheetAnimation(spriteSheetAnimation3.animationProperties);
    anim5 = SpriteSheetAnimation(spriteSheetAnimation4.animationProperties);
    anim6 = SpriteSheetAnimation(spriteSheetAnimation5.animationProperties);

    std::vector<std::string> instructions = {
        "1: Fireball",
        "2: Doom brick",
        "3: Explosive brick",
        "4: Aim Powerup",
        "5: Gun Powerup",
        "SPACE: Toggle Animation",
        "ESC: Quit"
    };

    // Alle Animationen starten
    anim2.playing = true;
    anim3.playing = true;
    anim4.playing = true;
    anim5.playing = true;
    anim6.playing = true;

    int currentTexture = 1; // Start mit Textur 1
    bool animationPaused = false;

    // Pointer auf die aktuell aktive Animation
    SpriteSheetAnimation *currentAnimation = &anim2;
    texture *currentTextureObj = &spriteSheetAnimation1;
    Uint32 lastTime = SDL_GetTicks();
    bool running = true;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (running) {
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
                            animationPaused = !animationPaused;
                            break;
                        case SDLK_1:
                            currentTexture = 1;
                            currentAnimation = &anim2;
                            currentTextureObj = &spriteSheetAnimation1;
                            break;
                        case SDLK_2:
                            currentTexture = 2;
                            currentAnimation = &anim3;
                            currentTextureObj = &spriteSheetAnimation2;
                            break;
                        case SDLK_3:
                            currentTexture = 3;
                            currentAnimation = &anim4;
                            currentTextureObj = &spriteSheetAnimation3;
                            break;
                        case SDLK_4:
                            currentTexture = 4;
                            currentAnimation = &anim5;
                            currentTextureObj = &spriteSheetAnimation4;
                            break;
                        case SDLK_5:
                            currentTexture = 5;
                            currentAnimation = &anim6;
                            currentTextureObj = &spriteSheetAnimation5;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        // Zeit für Animation aktualisieren
        const Uint32 currentTime = SDL_GetTicks();
        const float deltaTime = (currentTime - lastTime) / 1000.0f; // in Sekunden
        lastTime = currentTime;

        // Animation aktualisieren
        if (!animationPaused) {
            anim2.play(deltaTime);
            anim3.play(deltaTime);
            anim4.play(deltaTime);
            anim5.play(deltaTime);
            anim6.play(deltaTime);
        }

        // Rendern
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Hier die Textur zeichnen
        glEnable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Zeichnen der UV-Test-Textur im Hintergrund
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
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
        glTexCoord2f(currentAnimation->texturePosition[0], currentAnimation->texturePosition[1]);
        glVertex2f(-0.5f, -0.5f);

        // Bottom-right corner
        glTexCoord2f(currentAnimation->texturePosition[2], currentAnimation->texturePosition[3]);
        glVertex2f(0.5f, -0.5f);

        // Top-right corner
        glTexCoord2f(currentAnimation->texturePosition[4], currentAnimation->texturePosition[5]);
        glVertex2f(0.5f, 0.5f);

        // Top-left corner
        glTexCoord2f(currentAnimation->texturePosition[6], currentAnimation->texturePosition[7]);
        glVertex2f(-0.5f, 0.5f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float yPos = 0.9f;
        for (const auto &instruction: instructions) {
            textManager.write(instruction, Fonts::IntroDescription, false, 1.3f, -0.9f, yPos);
            yPos -= 0.07f;
        }

        std::string textureName;
        switch (currentTexture) {
            case 1: textureName = "Fireball";
                break;
            case 2: textureName = "Doom Brick";
                break;
            case 3: textureName = "Explosive Brick";
                break;
            case 4: textureName = "Aim Powerup";
                break;
            case 5: textureName = "Gun Powerup";
                break;
            default: textureName = "Unknown";
                break;
        }

        std::string statusText = "Current Texture: " + textureName;
        textManager.write(statusText, Fonts::Menu, true, 0.8f, 0.0f, -0.7f);

        std::string animationStatus = animationPaused ? "Animation: paused" : "Animation: running";
        textManager.write(animationStatus, Fonts::Menu, true, 0.7f, 0.0f, -0.8f);

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
