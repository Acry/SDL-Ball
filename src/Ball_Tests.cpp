#include <cstdlib>
#include "Paddle.h"
#include "TextureManager.h"
#include "Display.hpp"
#include "CollisionManager.h"

// Einfacher Mock für EventManager
class MockEventManager : public EventManager {
public:
    void emit(GameEvent event, const EventData &data) {
        // Einfach die Events protokollieren
        const char *eventNames[] = {"BallHitBorder", "BallLost", "BallHitBrick"};
        SDL_Log("Event ausgelöst: %s an Position (%.2f, %.2f)",
                eventNames[static_cast<int>(event)], data.posX, data.posY);
    }
};

GLfloat normalizedMouseX, normalizedMouseY;

int main() {
    // Display initialisieren
    Display display(0, 1280, 720, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }

    // TextureManager erstellen
    const TextureManager textureManager;

    // Mock EventManager erstellen
    MockEventManager eventManager;

    // CollisionManager erstellen
    // CollisionManager collisionManager;

    // Ball erstellen und initialisieren
    Ball ball(&eventManager);
    ball.setEyeCandy(true);

    // Ball-Textur laden
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

    // Ball einrichten
    // ball.setSize(0.03f);     // Größe setzen
    // ball.pos_x = 0.0f;       // Startposition
    // ball.pos_y = 0.0f;
    // ball.setSpeed(0.3f);     // Geschwindigkeit setzen
    // ball.setAngle(RAD / 4);  // 45° Startwinkel

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

    // Zusätzliche Texturen für die Layer (Glue und Gun)
    paddle.layerTex = new SpriteSheetAnimation[2];

    // Glue-Layer laden
    const std::filesystem::path glueTexPath = "../themes/default/gfx/paddle/glue.png";
    const std::filesystem::path gluePropsPath = "../themes/default/gfx/paddle/glue.txt";
    if (!textureManager.load(glueTexPath, paddle.layerTex[0])) {
        SDL_Log("Fehler beim Laden der Glue-Textur");
    } else {
        textureManager.readTexProps(gluePropsPath, paddle.layerTex[0]);
    }

    // Gun-Layer laden
    const std::filesystem::path gunTexPath = "../themes/default/gfx/paddle/gun.png";
    const std::filesystem::path gunPropsPath = "../themes/default/gfx/paddle/gun.txt";
    if (!textureManager.load(gunTexPath, paddle.layerTex[1])) {
        SDL_Log("Fehler beim Laden der Gun-Textur");
    } else {
        textureManager.readTexProps(gunPropsPath, paddle.layerTex[1]);
    }
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

        ball.update(deltaTime);
        paddle.update(deltaTime);

        CollisionPoint cp;
        if (ball.active && !ball.glued && CollisionManager::checkBallPaddleCollision(ball, paddle, cp)) {
            // Abprallwinkel wird bereits in checkBallPaddleCollision gesetzt

            // Wenn Paddle Klebeschicht hat, Ball festkleben
            if (paddle.hasGlueLayer) {
                ball.glued = true;
                ball.gluedX = ball.pos_x - paddle.pos_x;
            }
        }

        if (!ball.active) {
            ball.active = true;
            ball.pos_x = paddle.pos_x;
            ball.pos_y = paddle.pos_y + paddle.height + ball.height;
            ball.glued = true;
            ball.gluedX = 0.0f;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        paddle.draw(deltaTime);
        ball.draw(deltaTime);
        SDL_GL_SwapWindow(display.sdlWindow);
        SDL_Delay(16); // ~60fps
    }

    // Aufräumen
    delete[] paddle.layerTex;
    // delete[] ball.layerTex;

    return EXIT_SUCCESS;
}
