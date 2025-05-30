#include <cstdlib>

#include "Ball.h"
#include "Paddle.h"
#include "TextureManager.h"
#include "Display.hpp"
#include "difficulty_settings.h"
#include "PlayfieldBorder.h"
#include "MockEventManager.h"
#include "TestHelper.h"
#include "TextManager.h"

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(display.sdlWindow, "SDL-Ball: Ball Test");
    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
        return EXIT_FAILURE;
    }
    TestHelper testHelper(textManager);
    MockEventManager eventManager;
    const TextureManager textureManager;
    SpriteSheetAnimation tex;

    Ball ball(&eventManager);
    const std::filesystem::path ballTexPath = "../themes/default/gfx/ball/normal.png";
    const std::filesystem::path ballPropsPath = "../themes/default/gfx/ball/normal.txt";

    if (!textureManager.load(ballTexPath, ball.texture)) {
        SDL_Log("Fehler beim Laden der Ball-Textur: %s", ballTexPath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTextureProperties(ballPropsPath, ball.texture);

    // Feuertextur für explosiven Ball laden
    const std::filesystem::path fireTexPath = "../themes/default/gfx/ball/fireball.png";
    const std::filesystem::path firePropsPath = "../themes/default/gfx/ball/fireball.txt";
    if (!textureManager.load(fireTexPath, ball.fireTex)) {
        SDL_Log("Fehler beim Laden der Feuer-Textur");
    } else {
        textureManager.readTextureProperties(firePropsPath, ball.fireTex);
    }

    std::vector<std::string> instructions = {
        "1: Ball speed EASY",
        "2: Ball speed NORMAL",
        "3: Ball speed HARD",
        "e: Explosive ball",
        "g: Ball grow",
        "m: Toggle Mouse Coordinates",
        "p: Log ball speed",
        "r: Reset ball",
        "s: Ball shrink",
        "x: max ball speed",
        "ESC: Quit"
    };

    SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Uint32 lastTime = SDL_GetTicks();
    GLfloat normalizedMouseX = 0.0f, normalizedMouseY = 0.0f;
    bool running = true;

    float currentSpeed;
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
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (ball.glued) {
                        ball.launchFromPaddle();
                    }
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        ball.setSpeed(DifficultySettings::BallSpeed::EASY, DifficultySettings::MaxBallSpeed::EASY);
                        SDL_Log("Ballgeschwindigkeit: EASY (%.2f/%.2f)", ball.velocity,
                                DifficultySettings::MaxBallSpeed::EASY);
                        break;
                    case SDLK_2:
                        ball.setSpeed(DifficultySettings::BallSpeed::NORMAL, DifficultySettings::MaxBallSpeed::NORMAL);
                        SDL_Log("Ballgeschwindigkeit: NORMAL (%.2f/%.2f)", ball.velocity,
                                DifficultySettings::MaxBallSpeed::NORMAL);
                        break;
                    case SDLK_3:
                        ball.setSpeed(DifficultySettings::BallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD);
                        SDL_Log("Ballgeschwindigkeit: HARD (%.2f/%.2f)", ball.velocity,
                                DifficultySettings::MaxBallSpeed::HARD);
                        break;
                    case SDLK_e:
                        ball.explosive = !ball.explosive;
                        break;
                    case SDLK_g:
                        ball.grow(ball.getWidth() * 1.5f);
                        break;
                    case SDLK_s:
                        ball.grow(ball.getWidth() * 0.7f);
                        break;
                    case SDLK_m:
                        testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                        break;
                    case SDLK_x: // Maximalgeschwindigkeit testen
                        // Sehr hohen Wert setzen, der die Maximalgeschwindigkeit überschreiten sollte
                        ball.setSpeed(DifficultySettings::MaxBallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD);
                        // Dieser Wert sollte auf die maximale Geschwindigkeit begrenzt werden
                        SDL_Log("Ballgeschwindigkeit auf Maximum gesetzt: %.2f", ball.velocity);
                        break;
                    case SDLK_p:
                        currentSpeed = std::sqrt(ball.xvel * ball.xvel + ball.yvel * ball.yvel);
                        SDL_Log("Aktuelle Ballgeschwindigkeit: %.2f", currentSpeed);
                        break;
                    case SDLK_r:
                        ball.init();
                        break;
                    case SDLK_PLUS:
                    case SDLK_KP_PLUS:
                        // Erhöhe die Geschwindigkeit um einen kleinen Wert
                        currentSpeed = ball.velocity + 0.05f;
                        ball.setSpeed(currentSpeed, DifficultySettings::MaxBallSpeed::HARD);
                        break;
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS:
                        currentSpeed = std::max(0.05f, ball.velocity - 0.05f);
                        ball.setSpeed(currentSpeed, DifficultySettings::MaxBallSpeed::HARD);
                        SDL_Log("Ballgeschwindigkeit verringert: %.2f", ball.velocity);
                        break;
                    // k kill ball
                    default: ;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * (
                                       2.0f / display.viewportW);
                normalizedMouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 * (
                                       2.0f / display.viewportH);
            }
        }
        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);

        if (ball.active) {
            ball.update(deltaTime);
            // Horizontale Grenzen
            if (ball.pos_x < -1.0f) {
                ball.pos_x = -1.0f; // An linke Grenze zurücksetzen
                ball.xvel = -ball.xvel; // Geschwindigkeit umkehren mit leichtem Energieverlust
            } else if (ball.pos_x + ball.width > 1.0f) {
                ball.pos_x = 1.0f - ball.width; // An rechte Grenze zurücksetzen
                ball.xvel = -ball.xvel; // Geschwindigkeit umkehren mit leichtem Energieverlust
            }
            // Vertikale Grenzen
            if (ball.pos_y < -1.0f) {
                ball.pos_y = -1.0f; // An untere Grenze zurücksetzen
                ball.yvel = -ball.yvel; // Geschwindigkeit umkehren mit leichtem Energieverlust
            } else if (ball.pos_y + ball.height > 1.0f) {
                ball.pos_y = 1.0f - ball.height; // An obere Grenze zurücksetzen
                ball.yvel = -ball.yvel; // Geschwindigkeit umkehren mit leichtem Energieverlust
            }

        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (ball.active) {
            ball.draw(deltaTime);
        }
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

        char tempText2[64];
        sprintf(tempText2, "B: %.2f, %.2f", ball.pos_x, ball.pos_y);
        textManager.write(tempText2, Fonts::IntroDescription, false, 1.0f, ball.pos_x, ball.pos_y);
        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(display.sdlWindow);
    }
    return EXIT_SUCCESS;
}
