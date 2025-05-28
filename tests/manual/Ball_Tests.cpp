#include <cstdlib>

#include "Ball.h"
#include "Paddle.h"
#include "TextureManager.h"
#include "Display.hpp"
#include "difficulty_settings.h"
#include "PlayfieldBorder.h"
#include "MockEventManager.h"
#include "TextManager.h"

GLfloat normalizedMouseX, normalizedMouseY;

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
    }
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

    Paddle paddle(&eventManager);
    const std::filesystem::path texturePath = "../themes/default/gfx/paddle/base.png";
    const std::filesystem::path propsPath = "../themes/default/gfx/paddle/base.txt";

    if (!textureManager.load(texturePath, paddle.texture)) {
        SDL_Log("Fehler beim Laden der Paddle-Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTextureProperties(propsPath, paddle.texture);

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
    PlayfieldBorder topBorder(PlayfieldBorder::Side::Top, texBorder, &eventManager);

    std::vector<std::string> instructions = {
        "1: Ball speed EASY",
        "2: Ball speed NORMAL",
        "3: Ball speed HARD",
        "e: Explosive ball",
        "r: Reset ball",
        "g: Ball grow",
        "s: Ball shrink",
        "m: max ball speed",
        "p: Log ball speed",
        "ESC: Quit"
    };

    SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Uint32 lastTime = SDL_GetTicks();
    bool running = true;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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
                    // Ball vom Paddle lösen wenn geklebt
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
                    case SDLK_e:
                        ball.explosive = !ball.explosive;
                        break;
                    case SDLK_g:
                        ball.grow(ball.getWidth() * 1.5f);
                        break;
                    case SDLK_s:
                        ball.grow(ball.getWidth() * 0.7f);
                        break;
                    case SDLK_m: // Maximalgeschwindigkeit testen
                        // Sehr hohen Wert setzen, der die Maximalgeschwindigkeit überschreiten sollte
                        ball.setSpeed(DifficultySettings::MaxBallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD);
                        // Dieser Wert sollte auf die maximale Geschwindigkeit begrenzt werden
                        SDL_Log("Ballgeschwindigkeit auf Maximum gesetzt: %.2f", ball.velocity);
                        break;
                    case SDLK_p: // Aktuelle Geschwindigkeit ausgeben
                        SDL_Log("Aktuelle Ballgeschwindigkeit: %.2f", ball.velocity);
                        break;
                    case SDLK_r: // reset ball
                        ball.init();
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
        if (ball.active) {
            ball.update(deltaTime);
        }
        if (paddle.active) {
            paddle.update(deltaTime);
        }
        if (ball.glued) {
            ball.pos_y = paddle.pos_y + paddle.height;
            ball.pos_x = paddle.pos_x + paddle.getWidth() / 2.0f - ball.getWidth() / 2.0f;
        }

        if (paddle.hasGlueLayer) {
            ball.glued = true;
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spielfeldränder zeichnen
        leftBorder.draw(deltaTime);
        rightBorder.draw(deltaTime);
        topBorder.draw(deltaTime); // should not be visible
        if (paddle.active) {
            paddle.draw(deltaTime);
        }

        if (ball.active) {
            ball.draw(deltaTime);
        }
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
    return EXIT_SUCCESS;
}
