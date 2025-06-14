// Collision_Tests.cpp
#include <cstdlib>
#include <SDL2/SDL_log.h>

#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "TextureManager.h"
#include "DisplayManager.hpp"
#include "CollisionManager.h"
#include "difficulty_settings.h"
#include "PlayfieldBorder.h"
#include "SoundManager.h"
#include "TestHelper.h"
#include "TextManager.h"

#undef EASY
#undef NORMAL
#undef HARD

#define DEBUG_DRAW_BALL_BOUNDING_BOXES 0

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Collision Test");

    TextManager textManager;
    if (!textManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }

    TestHelper testHelper(textManager, &eventManager);

    const CollisionManager collisionManager(&eventManager);

    SoundManager soundManager;
    if (!soundManager.setTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Sound-Themes");
    }
    soundManager.registerEvents(&eventManager);

    TextureManager textureManager;
    if (!textureManager.setSpriteTheme("../themes/default")) {
        SDL_Log("Fehler beim Laden des Texture-Themes");
        return EXIT_FAILURE;
    }

    // Brick brick;
    std::vector<Brick> bricks;
    constexpr int BRICK_ROWS = 3;
    // Bricks initialisieren
    for (int row = 0; row < BRICK_ROWS; row++) {
        constexpr int BRICK_COLS = 7;
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick brick(&eventManager);
            // Positionen berechnen (Anpassung nach Bedarf)
            float x = -0.8f + col * 0.25f;
            float y = 0.5f - row * 0.12f;
            brick.setPosition(x, y);
            brick.texture = textureManager.getBrickTexture(BrickTexture::Base);
            bricks.push_back(brick);
        }
    }

    Ball ball(&eventManager);
    ball.texture = textureManager.getBallTexture(BallTexture::Normal);

    Paddle paddle(&eventManager);
    paddle.texture = textureManager.getPaddleTexture(PaddleTexture::Base);

    PlayfieldBorder leftBorder(PlayfieldBorder::Side::Left, &eventManager);
    leftBorder.texture = textureManager.getMiscTexture(MiscTexture::Border);
    leftBorder.createDisplayList();
    PlayfieldBorder rightBorder(PlayfieldBorder::Side::Right, &eventManager);
    rightBorder.texture = textureManager.getMiscTexture(MiscTexture::Border);
    rightBorder.createDisplayList();
    PlayfieldBorder topBorder(PlayfieldBorder::Side::Top, &eventManager);
    topBorder.texture = textureManager.getMiscTexture(MiscTexture::Border);
    topBorder.createDisplayList();

    std::vector<std::string> instructions = {
        "1: Ball speed EASY",
        "2: Ball speed NORMAL",
        "3: Ball speed HARD",
        "r: Reset ball",
        "g: Ball grow",
        "s: Ball shrink",
        "m: max ball speed",
        "p: Log ball speed",
        "ESC: Quit",
        "A: Auto-Paddle"
    };

    SDL_WarpMouseInWindow(displayManager.sdlWindow, displayManager.currentW / 2, displayManager.currentH / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Uint32 lastTime = SDL_GetTicks();
    bool running = true;
    GLfloat normalizedMouseX, normalizedMouseY;
    bool update = true;
    bool autoPaddle = false;

    float currentSpeed;
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
                float moveStep = 0.09f; // Konstanter, kleiner Wert für Tastatureingabe
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        paddle.moveTo(paddle.pos_x - moveStep, deltaTime);
                        break;
                    case SDLK_RIGHT:
                        paddle.moveTo(paddle.pos_x + moveStep, deltaTime);
                        break;
                    // Für Taste 1: EASY-Einstellungen
                    case SDLK_1:
                        ball.setSpeed(DifficultySettings::BallSpeed::EASY, DifficultySettings::MaxBallSpeed::EASY);
                        SDL_Log("Ballgeschwindigkeit: EASY (%.2f/%.2f)", ball.velocity,
                                DifficultySettings::MaxBallSpeed::EASY);
                        break;
                    // Für Taste 2: NORMAL-Einstellungen
                    case SDLK_2:
                        ball.setSpeed(DifficultySettings::BallSpeed::NORMAL, DifficultySettings::MaxBallSpeed::NORMAL);
                        SDL_Log("Ballgeschwindigkeit: NORMAL (%.2f/%.2f)", ball.velocity,
                                DifficultySettings::MaxBallSpeed::NORMAL);
                        break;
                    // Für Taste 3: HARD-Einstellungen
                    case SDLK_3:
                        ball.setSpeed(DifficultySettings::BallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD);
                        SDL_Log("Ballgeschwindigkeit: HARD (%.2f/%.2f)", ball.velocity,
                                DifficultySettings::MaxBallSpeed::HARD);
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
                    case SDLK_p: // Aktuelle Geschwindigkeit ausgeben
                        currentSpeed = std::sqrt(ball.xvel * ball.xvel + ball.yvel * ball.yvel);
                        SDL_Log("Aktuelle Ballgeschwindigkeit: %.2f", currentSpeed);
                        break;
                    case SDLK_r: // reset ball
                        ball.init();
                        break;
                    case SDLK_u: // toggle update
                        update = !update;
                        break;
                    case SDLK_x: // Maximalgeschwindigkeit testen
                        // Sehr hohen Wert setzen, der die Maximalgeschwindigkeit überschreiten sollte
                        ball.setSpeed(DifficultySettings::MaxBallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD);
                        // Dieser Wert sollte auf die maximale Geschwindigkeit begrenzt werden
                        SDL_Log("Ballgeschwindigkeit auf Maximum gesetzt: %.2f", ball.velocity);
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
                        break;
                    case SDLK_a:
                        autoPaddle = !autoPaddle;
                        break;
                    default: ;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - displayManager.viewportX - displayManager.viewportW / 2.0f) * (
                                       2.0f / displayManager.viewportW);
                normalizedMouseY = (event.motion.y - displayManager.viewportY - displayManager.viewportH / 2.0f) * -1 * (
                                       2.0f / displayManager.viewportH);
            }
            paddle.moveTo(normalizedMouseX, deltaTime);
        }
        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);

#pragma region update
        if (update) {
            if (ball.active) {
                ball.update(deltaTime);
            }
            paddle.update(deltaTime);

            // Auto-Paddle-Mechanik
            if (autoPaddle) {
                // Ballflugbahn vorhersagen statt nur aktuelle Position zu verfolgen
                float predictedX = ball.pos_x;
                float predictedY = ball.pos_y;
                float velX = ball.xvel;
                float velY = ball.yvel;

                // Nur für aufsteigende Bälle die Vorhersage berechnen
                if (velY > 0) {
                    // Einfache Vorhersage: Wie weit fliegt der Ball horizontal,
                    // bis er die Paddlehöhe erreicht?
                    if (float timeToReachPaddle = (paddle.pos_y - predictedY) / velY; timeToReachPaddle > 0) {
                        predictedX += velX * timeToReachPaddle;
                    }
                }

                // Kleiner zufälliger Fehler für realistischeres Verhalten (nur alle 0.5 Sekunden neu berechnen)
                static float lastErrorTime = 0.0f;
                static float errorOffset = 0.0f;

                if (float autoPaddleTime = 0.0f; autoPaddleTime - lastErrorTime > 0.5f) {
                    lastErrorTime = autoPaddleTime;
                    errorOffset = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
                }

                // Paddlemitte zur vorhergesagten Ballposition bewegen, mit kleinem Fehler
                float target = (predictedX + ball.width / 2.0f + errorOffset) - paddle.getWidth() / 2.0f;

                // Auf Spielfeldgrenzen beschränken
                if (target < -1.0f) target = -1.0f;
                if (target > 1.0f - paddle.getWidth()) target = 1.0f - paddle.getWidth();

                // Sanfte Bewegung zum Ziel
                float currentX = paddle.pos_x;
                float moveSpeed = 1.5f; // Anpassbarer Wert - höher = schnellere Reaktion
                float newX = currentX + (target - currentX) * moveSpeed * deltaTime * 10.0f;

                paddle.moveTo(newX, deltaTime);
            }

            if (ball.glued) {
                ball.pos_y = paddle.pos_y + paddle.height;
                ball.pos_x = paddle.pos_x + paddle.getWidth() / 2.0f - ball.getWidth() / 2.0f;
            }


            soundManager.play();
        }
#pragma endregion update

#pragma region draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (ball.active) {
            paddle.draw(deltaTime);
            ball.draw(deltaTime);
        }
        for (auto &brick: bricks) {
            if (brick.isActive()) {
                brick.draw(deltaTime);
            }
        }
        leftBorder.draw(deltaTime);
        rightBorder.draw(deltaTime);
        topBorder.draw(deltaTime); // should not be visible
#if DEBUG_DRAW_BALL_BOUNDING_BOXES
        GLfloat oldColor[4];
        glGetFloatv(GL_CURRENT_COLOR, oldColor);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f); // Weiß für die Paddle-Farbe
        glBegin(GL_QUADS);
        // unten links
        glVertex3f(ball.pos_x, ball.pos_y + ball.height, 0.0f);
        // unten rechts
        glVertex3f(ball.pos_x + ball.width, ball.pos_y + ball.height, 0.0f);
        // oben rechts
        glVertex3f(ball.pos_x + ball.width, ball.pos_y, 0.0f);
        // oben links
        glVertex3f(ball.pos_x, ball.pos_y, 0.0f);
        glEnd();
        glDisable(GL_BLEND);
        glColor4fv(oldColor);
#endif

        testHelper.renderInstructions(deltaTime, instructions);

        GLfloat oldColor2[4];
        glGetFloatv(GL_CURRENT_COLOR, oldColor2);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        char tempText2[64];
        sprintf(tempText2, "B: %.2f, %.2f", ball.pos_x, ball.pos_y);
        textManager.write(tempText2, Fonts::IntroDescription, false, 1.0f, ball.pos_x, ball.pos_y);

        char tempText3[64];
        sprintf(tempText3, "P: %.2f, %.2f", paddle.pos_x, paddle.pos_y);
        textManager.write(tempText3, Fonts::IntroDescription, false, 1.0f, paddle.pos_x, paddle.pos_y);
        glColor4fv(oldColor2);
        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
#pragma endregion draw
    }
    return EXIT_SUCCESS;
}
