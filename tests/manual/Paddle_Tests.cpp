// Paddle_Tests.cpp
#include <cstdlib>

#include "DisplayManager.hpp"
#include "Paddle.h"
#include "PlayfieldBorder.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

#define DEBUG_DRAW_PADDLE_BOUNDING_BOXES 0

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Paddle Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    TestHelper testHelper(textManager, &eventManager);
    Paddle paddle(&eventManager);
    TextureManager textureManager;
    if (!textureManager.setSpriteTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Texture-Themes");
        return EXIT_FAILURE;
    }

    paddle.texture = textureManager.getPaddleTexture(PaddleTexture::Base);
    if (!paddle.texture) {
        SDL_Log("Fehler beim Laden der Paddle-Textur");
        return EXIT_FAILURE;
    }

    // Array-Deklaration entfernen, da layerTex bereits als Array in der Klasse definiert ist
    paddle.layerTex[0] = textureManager.getPaddleTexture(PaddleTexture::Glue);
    if (!paddle.layerTex[0]) {
        SDL_Log("Fehler beim Laden der Glue-Textur");
        return EXIT_FAILURE;
    }

    paddle.layerTex[1] = textureManager.getPaddleTexture(PaddleTexture::Gun);
    if (!paddle.layerTex[1]) {
        SDL_Log("Fehler beim Laden der Gun-Textur");
        return EXIT_FAILURE;
    }

    std::vector<std::string> instructions = {
        "1: Set Paddle Glue Layer",
        "2: Set Paddle Gun Layer",
        "g: Paddle grow",
        "s: Paddle shrink",
        "<-: Paddle move left",
        "->: Paddle move right",
        "Mouse: Move Paddle",
        "m: Toggle Mouse Coordinates",
        "ESC: Quit"
    };

    SDL_WarpMouseInWindow(displayManager.sdlWindow, displayManager.currentW / 2, displayManager.currentH / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Uint32 lastTime = SDL_GetTicks();
    GLfloat normalizedMouseX, normalizedMouseY;
    bool running = true;
    while (running) {
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
                    displayManager.resize(event.window.data1, event.window.data2);
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
                    case SDLK_g:
                        paddle.grow(paddle.getWidth() * 1.5f);
                        break;
                    case SDLK_s:
                        paddle.grow(paddle.getWidth() * 0.7f);
                    case SDLK_m:
                        testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                        break;
                    case SDLK_1:
                        paddle.setGlueLayer(!paddle.hasGlueLayer);
                        break;
                    case SDLK_2:
                        paddle.setGunLayer(!paddle.hasGunLayer);
                        break;
                    default: ;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - displayManager.viewportX - displayManager.viewportW / 2.0f) * (
                                       2.0f / displayManager.viewportW);
                normalizedMouseY = (event.motion.y - displayManager.viewportY - displayManager.viewportH / 2.0f) * -1 *
                                   (
                                       2.0f / displayManager.viewportH);
                paddle.moveTo(normalizedMouseX, deltaTime);
            }
        }
        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);
        paddle.update(deltaTime);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (paddle.pos_x + paddle.width > +1.0f) {
            paddle.pos_x = 1.0f - paddle.width;
        }
        paddle.draw(deltaTime);
#if DEBUG_DRAW_PADDLE_BOUNDING_BOXES
        GLfloat oldColor[4];
        glGetFloatv(GL_CURRENT_COLOR, oldColor);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

        glBegin(GL_QUADS);

        // Bottom-left corner.
        glVertex3f(paddle.pos_x, paddle.pos_y, 0.0f);
        // Bottom-right corner.
        glVertex3f(paddle.pos_x + paddle.width, paddle.pos_y, 0.0f);
        // Top-right corner.
        glVertex3f(paddle.pos_x + paddle.width, paddle.pos_y + paddle.height, 0.0f);
        // Top-left corner.
        glVertex3f(paddle.pos_x, paddle.pos_y + paddle.height, 0.0f);

        glEnd();

        glDisable(GL_BLEND);
        glColor4fv(oldColor);
#endif
        testHelper.renderInstructions(deltaTime, instructions);
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
        testHelper.drawMouseCoordinates();
        char tempText3[64];
        sprintf(tempText3, "P: %.2f, %.2f", paddle.pos_x, paddle.pos_y);
        textManager.write(tempText3, Fonts::IntroDescription, false, 1.0f, paddle.pos_x, paddle.pos_y);
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
