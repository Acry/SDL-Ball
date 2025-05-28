// Paddle_Tests.cpp
#include <cstdlib>

#include "Paddle.h"
#include "TextureManager.h"
#include "Display.hpp"
#include "PlayfieldBorder.h"
#include "MockEventManager.h"
#include "TextManager.h"

#define DEBUG_DRAW_PADDLE_BOUNDING_BOXES 0

GLfloat normalizedMouseX, normalizedMouseY;

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: Paddle Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    MockEventManager eventManager;
    Paddle paddle(&eventManager);
    const TextureManager textureManager;

    // Basis-Textur für das Paddle laden
    const std::filesystem::path texturePath = "../themes/default/gfx/paddle/base.png";
    const std::filesystem::path propsPath = "../themes/default/gfx/paddle/base.txt";

    if (!textureManager.load(texturePath, paddle.texture)) {
        SDL_Log("Fehler beim Laden der Paddle-Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTextureProperties(propsPath, paddle.texture);

    // Zusätzliche Texturen für die Layer (Glue und Gun)
    paddle.layerTex = new SpriteSheetAnimation[2];

    // Glue-Layer laden
    const std::filesystem::path glueTexPath = "../themes/default/gfx/paddle/glue.png";
    const std::filesystem::path gluePropsPath = "../themes/default/gfx/paddle/glue.txt";
    if (!textureManager.load(glueTexPath, paddle.layerTex[0])) {
        SDL_Log("Fehler beim Laden der Glue-Textur");
    } else {
        textureManager.readTextureProperties(gluePropsPath, paddle.layerTex[0]);
    }

    // Gun-Layer laden
    const std::filesystem::path gunTexPath = "../themes/default/gfx/paddle/gun.png";
    const std::filesystem::path gunPropsPath = "../themes/default/gfx/paddle/gun.txt";
    if (!textureManager.load(gunTexPath, paddle.layerTex[1])) {
        SDL_Log("Fehler beim Laden der Gun-Textur");
    } else {
        textureManager.readTextureProperties(gunPropsPath, paddle.layerTex[1]);
    }

    // PlayfieldBorder-Textur laden
    SpriteSheetAnimation texBorder;
    const std::filesystem::path borderTexPath = "../themes/default/gfx/border.png";
    const std::filesystem::path borderPropsPath = "../themes/default/gfx/border.txt";
    if (!textureManager.load(borderTexPath, texBorder)) {
        SDL_Log("Fehler beim Laden der Border-Textur: %s", borderTexPath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTextureProperties(borderPropsPath, texBorder);

    // PlayfieldBorder-Objekte erzeugen
    PlayfieldBorder leftBorder(PlayfieldBorder::Side::Left, texBorder, &eventManager);
    PlayfieldBorder rightBorder(PlayfieldBorder::Side::Right, texBorder, &eventManager);

    std::vector<std::string> instructions = {
        "1: Set Paddle Glue Layer",
        "2: Set Paddle Gun Layer",
        "g: Paddle grow",
        "s: Paddle shrink",
        "<-: Padle move left",
        "->: Paddle move right",
        "Mouse: Move Paddle",
        "ESC: Quit"
    };

    SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Uint32 lastTime = SDL_GetTicks();
    bool running = true;
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while (running) {
        // Zeit für Animation und Update aktualisieren
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
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
                }
                float moveStep = 0.09f; // Konstanter, kleiner Wert für Tastatureingabe
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        paddle.moveTo(paddle.pos_x - moveStep, deltaTime);
                        break;
                    case SDLK_RIGHT:
                        paddle.moveTo(paddle.pos_x + moveStep, deltaTime);
                        break;
                    case SDLK_g: // Paddle wachsen lassen
                        paddle.grow(paddle.getWidth() * 1.5f);
                        break;
                    case SDLK_s: // Paddle verkleinern
                        paddle.grow(paddle.getWidth() * 0.7f);
                        break;
                    case SDLK_1: // Glue aktivieren/deaktivieren
                        paddle.setGlueLayer(!paddle.hasGlueLayer);
                        break;
                    case SDLK_2: // Gun aktivieren/deaktivieren
                        paddle.setGunLayer(!paddle.hasGunLayer);
                        break;
                    default: ;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - display.currentW / 2.0f);
                normalizedMouseY = (event.motion.y - display.currentH / 2.0f) * -1;
                normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * (
                                       2.0f / display.viewportW);
                normalizedMouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 * (
                                       2.0f / display.viewportH);
                normalizedMouseX = std::max(-1.0f, std::min(1.0f, normalizedMouseX));
                normalizedMouseY = std::max(-1.0f, std::min(1.0f, normalizedMouseY));
                paddle.moveTo(normalizedMouseX, deltaTime);
            }
        }
        paddle.update(deltaTime);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        leftBorder.draw(deltaTime);
        rightBorder.draw(deltaTime);

        paddle.draw(deltaTime);
#if DEBUG_DRAW_PADDLE_BOUNDING_BOXES
        GLfloat oldColor[4];
        glGetFloatv(GL_CURRENT_COLOR, oldColor);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f); // Weiß für die Paddle-Farbe
        glBegin(GL_QUADS);
        // unten links
        glVertex3f(paddle.pos_x, paddle.pos_y + paddle.height, 0.0f);
        // unten rechts
        glVertex3f(paddle.pos_x + paddle.width, paddle.pos_y + paddle.height, 0.0f);
        // oben rechts
        glVertex3f(paddle.pos_x + paddle.width, paddle.pos_y, 0.0f);
        // oben links
        glVertex3f(paddle.pos_x, paddle.pos_y, 0.0f);
        glEnd();
        glDisable(GL_BLEND);
        glColor4fv(oldColor);
#endif
        float yPos = 0.9f;
        constexpr auto currentFont = Fonts::Menu;
        const auto height = textManager.getHeight(currentFont);
        const auto offest = height;
        for (const auto &instruction: instructions) {
            textManager.write(instruction, currentFont, true, 1.0f, 0.0f, yPos);
            yPos -= height + offest;
        }
        // Red lines
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);

        glPushMatrix();
        glLoadIdentity();
        glLineWidth(1.0f);

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Rot (R,G,B,A)
        glBegin(GL_LINES);
        glVertex3f(-0.9f, 0.0f, 0.0f); // Startpunkt der Linie
        glVertex3f(0.9f, 0.0f, 0.0f); // Endpunkt der Linie
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glLoadIdentity();
        glLineWidth(0.01f);

        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f); // Startpunkt in der Mitte
        glVertex3f(0.0f, -1.0f, 0.0f);
        glEnd();
        glPopMatrix();
        SDL_GL_SwapWindow(display.sdlWindow);
    }

    // Aufräumen
    delete[] paddle.layerTex;

    return EXIT_SUCCESS;
}
