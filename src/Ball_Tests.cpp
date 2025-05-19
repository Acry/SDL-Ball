#include <cstdlib>
#include "Paddle.h"
#include "TextureManager.h"
#include "Display.hpp"
#include "CollisionManager.h"
#include "difficulty_settings.h"


class MockEventManager : public EventManager {
public:
    int eventCount = 0;  // Zählt ausgelöste Events

    void emit(GameEvent event, const EventData &data) override{
        eventCount++;
        const char *eventNames[] = {"BallHitBorder", "BallLost", "BallHitPaddle", "BrickDestroyed",
                                   "PowerUpCollected", "LevelCompleted", "GameOver"};

        SDL_Log("Event ausgelöst: %s (#%d) an Position (%.2f, %.2f)",
                eventNames[static_cast<int>(event)], eventCount,
                data.posX, data.posY);

        if (data.soundID >= 0) {
            SDL_Log("Sound-ID: %d", data.soundID);
        }
    }
};

GLfloat normalizedMouseX, normalizedMouseY;

int main() {
    Display display(0, 1280, 720, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }

    MockEventManager eventManager;
    Ball ball(&eventManager);

    const TextureManager textureManager;
    SpriteSheetAnimation tex;

    const std::filesystem::path ballTexPath = "../themes/default/gfx/ball/normal.png";
    const std::filesystem::path ballPropsPath = "../themes/default/gfx/ball/normal.txt";

    if (!textureManager.load(ballTexPath, ball.texture)) {
        SDL_Log("Fehler beim Laden der Ball-Textur: %s", ballTexPath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTexProps(ballPropsPath, ball.texture);

    // Feuertextur für explosiven Ball laden
    const std::filesystem::path fireTexPath = "../themes/default/gfx/ball/fireball.png";
    const std::filesystem::path firePropsPath = "../themes/default/gfx/ball/fireball.txt";
    if (!textureManager.load(fireTexPath, ball.fireTex)) {
        SDL_Log("Fehler beim Laden der Feuer-Textur");
    } else {
        textureManager.readTexProps(firePropsPath, ball.fireTex);
    }

    // Ball-Textur für den Tracer laden
    const std::filesystem::path tracerTexPath = "../themes/default/gfx/ball/tail.png";
    const std::filesystem::path tracerPropsPath = "../themes/default/gfx/ball/tail.txt";

    if (!textureManager.load(tracerTexPath, ball.tracer.tex)) {
        SDL_Log("Fehler beim Laden der Tracer-Textur");
    } else {
        textureManager.readTexProps(tracerPropsPath, ball.tracer.tex);
    }

    // Paddle erstellen und initialisieren
    Paddle paddle;

    // Basis-Textur für das Paddle laden
    const std::filesystem::path texturePath = "../themes/default/gfx/paddle/base.png";
    const std::filesystem::path propsPath = "../themes/default/gfx/paddle/base.txt";

    if (!textureManager.load(texturePath, paddle.texture)) {
        SDL_Log("Fehler beim Laden der Paddle-Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTexProps(propsPath, paddle.texture);

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
                        SDL_Log("Ballgeschwindigkeit: EASY (%.2f/%.2f)", ball.velocity, DifficultySettings::MaxBallSpeed::EASY);
                        break;
                        // Für Taste 2: NORMAL-Einstellungen
                    case SDLK_2:
                        ball.setSpeed(DifficultySettings::BallSpeed::NORMAL, DifficultySettings::MaxBallSpeed::NORMAL);
                        SDL_Log("Ballgeschwindigkeit: NORMAL (%.2f/%.2f)", ball.velocity, DifficultySettings::MaxBallSpeed::NORMAL);
                        break;
                        // Für Taste 3: HARD-Einstellungen
                    case SDLK_3:
                        ball.setSpeed(DifficultySettings::BallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD);
                        SDL_Log("Ballgeschwindigkeit: HARD (%.2f/%.2f)", ball.velocity, DifficultySettings::MaxBallSpeed::HARD);
                        break;
                    case SDLK_g: // Paddle wachsen lassen
                        ball.grow(ball.getWidth() * 1.5f);
                        break;
                    case SDLK_s: // Paddle verkleinern
                        ball.grow(ball.getWidth() * 0.7f);
                        break;
                    case SDLK_m: // Maximalgeschwindigkeit testen
                        // Sehr hohen Wert setzen, der die Maximalgeschwindigkeit überschreiten sollte
                        ball.setSpeed(DifficultySettings::MaxBallSpeed::HARD, DifficultySettings::MaxBallSpeed::HARD); // Dieser Wert sollte auf die maximale Geschwindigkeit begrenzt werden
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

        paddle.update(deltaTime);
        if (ball.glued) {
            ball.pos_x = paddle.pos_x + ball.gluedX;
            ball.pos_y = paddle.pos_y + paddle.height + ball.height;
        }
        CollisionPoint cp;
        if (ball.active && !ball.glued && CollisionManager::checkBallPaddleCollision(ball, paddle, cp)) {
            // Abprallwinkel wird bereits in checkBallPaddleCollision gesetzt

            // Wenn Paddle Klebeschicht hat, Ball festkleben
            if (paddle.hasGlueLayer) {
                ball.glued = true;
                ball.gluedX = ball.pos_x - paddle.pos_x;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        paddle.draw(deltaTime);
        if (ball.active) {
            ball.draw(deltaTime);
        }
        SDL_GL_SwapWindow(display.sdlWindow);
        SDL_Delay(16); // ~60fps
    }
    return EXIT_SUCCESS;
}
