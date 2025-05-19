#include <cstdlib>

#include "TextureManager.h"
#include "Display.hpp"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SpriteSheetAnimation spriteSheetAnimation;
    const TextureManager textureManager;
    // Pfad zu einer Textur-Datei und zugehörigen Eigenschaften
    const std::filesystem::path texturePath = "../themes/default/gfx/powerup/gun.png";
    const std::filesystem::path propsPath = "../themes/default/gfx/powerup/gun.txt";

    // Textur laden
    if (!textureManager.load(texturePath, spriteSheetAnimation)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }
    // Textureigenschaften laden
    textureManager.readTexProps(propsPath, spriteSheetAnimation);

    // Animation aktivieren
    spriteSheetAnimation.playing = true;
    spriteSheetAnimation.textureProperties.playing = true;

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
                if (event.key.keysym.sym == SDLK_t) {
                    // do something
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }
        // Zeit für Animation aktualisieren
        const Uint32 currentTime = SDL_GetTicks();
        const float deltaTime = (currentTime - lastTime) / 1000.0f; // in Sekunden
        lastTime = currentTime;

        // Animation aktualisieren
        spriteSheetAnimation.play(deltaTime);

        // Rendern
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Hier die Textur zeichnen
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, spriteSheetAnimation.textureProperties.texture);

        glColor4f(
            spriteSheetAnimation.textureProperties.glTexColorInfo[0],
            spriteSheetAnimation.textureProperties.glTexColorInfo[1],
            spriteSheetAnimation.textureProperties.glTexColorInfo[2],
            spriteSheetAnimation.textureProperties.glTexColorInfo[3]
        );

        // Ein Rechteck mit der Textur zeichnen
        glBegin(GL_QUADS);
        glTexCoord2f(spriteSheetAnimation.texturePosition[0], spriteSheetAnimation.texturePosition[1]);
        glVertex2f(-0.5f, 0.5f);

        glTexCoord2f(spriteSheetAnimation.texturePosition[2], spriteSheetAnimation.texturePosition[3]);
        glVertex2f(0.5f, 0.5f);

        glTexCoord2f(spriteSheetAnimation.texturePosition[4], spriteSheetAnimation.texturePosition[5]);
        glVertex2f(0.5f, -0.5f);

        glTexCoord2f(spriteSheetAnimation.texturePosition[6], spriteSheetAnimation.texturePosition[7]);
        glVertex2f(-0.5f, -0.5f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        SDL_Delay(13);
        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
