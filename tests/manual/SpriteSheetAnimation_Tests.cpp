#include <cstdlib>

#include "TextureManager.h"
#include "Display.hpp"
#include "TextManager.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: SpriteSheet Animation Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    SpriteSheetAnimation uvTest;

    SpriteSheetAnimation spriteSheetAnimation1;
    SpriteSheetAnimation spriteSheetAnimation2;
    SpriteSheetAnimation spriteSheetAnimation3;
    SpriteSheetAnimation spriteSheetAnimation4;
    SpriteSheetAnimation spriteSheetAnimation5;

    const std::filesystem::path texturePath1 = "../themes/default/gfx/ball/fireball";
    const std::filesystem::path texturePath2 = "../themes/default/gfx/brick/doom";
    const std::filesystem::path texturePath3 = "../themes/default/gfx/brick/explosive";
    const std::filesystem::path texturePath4 = "../themes/default/gfx/powerup/aim";
    const std::filesystem::path texturePath5 = "../themes/default/gfx/powerup/gun";

    // TextureManager erstellen
    const TextureManager textureManager;

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
    spriteSheetAnimation1.textureProperties.playing = true;
    spriteSheetAnimation2.textureProperties.playing = true;
    spriteSheetAnimation3.textureProperties.playing = true;
    spriteSheetAnimation4.textureProperties.playing = true;
    spriteSheetAnimation5.textureProperties.playing = true;
    int currentTexture = 1; // Start mit Textur 1
    bool animationPaused = false;

    // Pointer auf die aktuell aktive Animation
    SpriteSheetAnimation *currentAnimation = &spriteSheetAnimation1;
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
                    display.resize(event.window.data1, event.window.data2);
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
                            currentAnimation = &spriteSheetAnimation1;
                            break;
                        case SDLK_2:
                            currentTexture = 2;
                            currentAnimation = &spriteSheetAnimation2;
                            break;
                        case SDLK_3:
                            currentTexture = 3;
                            currentAnimation = &spriteSheetAnimation3;
                            break;
                        case SDLK_4:
                            currentTexture = 4;
                            currentAnimation = &spriteSheetAnimation4;
                            break;
                        case SDLK_5:
                            currentTexture = 5;
                            currentAnimation = &spriteSheetAnimation5;
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
            spriteSheetAnimation1.play(deltaTime);
            spriteSheetAnimation2.play(deltaTime);
            spriteSheetAnimation3.play(deltaTime);
            spriteSheetAnimation4.play(deltaTime);
            spriteSheetAnimation5.play(deltaTime);
        }

        // Rendern
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Hier die Textur zeichnen
        glEnable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Zeichnen der UV-Test-Textur im Hintergrund
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glBindTexture(GL_TEXTURE_2D, uvTest.textureProperties.texture);
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

        glBindTexture(GL_TEXTURE_2D, currentAnimation->textureProperties.texture);

        // Mit den Texturfarben färben
        glColor4fv(currentAnimation->textureProperties.glTexColorInfo);

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

        SDL_GL_SwapWindow(display.sdlWindow);
    }
    glDeleteTextures(1, &uvTest.textureProperties.texture);
    glDeleteTextures(1, &spriteSheetAnimation1.textureProperties.texture);
    glDeleteTextures(1, &spriteSheetAnimation2.textureProperties.texture);
    glDeleteTextures(1, &spriteSheetAnimation3.textureProperties.texture);
    glDeleteTextures(1, &spriteSheetAnimation4.textureProperties.texture);
    glDeleteTextures(1, &spriteSheetAnimation5.textureProperties.texture);

    return EXIT_SUCCESS;
}
