// PlayfieldBorder_Tests.cpp
#include <cstdlib>

#include "TextureManager.h"
#include "Display.hpp"
#include "PlayfieldBorder.h"
#include "TextManager.h"
#include "EventManager.h"
#include "ICollideable.h"
#include "MockEventManager.h"

class ICollideable;

// Mock-Klassen für Tests
class MockBall : public ICollideable {
public:
    float posX = 0.0f;
    float posY = 0.0f;

    float getPosX() const override { return posX; }
    float getPosY() const override { return posY; }
    float getWidth() const override { return 0.018f; }
    float getHeight() const override { return 0.018f; }
    bool isActive() const override { return true; }
    int getCollisionType() const override { return static_cast<int>(CollisionType::Ball); }

    void onCollision(ICollideable *, float, float) override {
    }
};

class MockPaddle : public ICollideable {
public:
    float posX = 0.0f;
    float posY = -0.9f;

    float getPosX() const override { return posX; }
    float getPosY() const override { return posY; }
    float getWidth() const override { return 0.062f; }
    float getHeight() const override { return 0.016f; }
    bool isActive() const override { return true; }
    int getCollisionType() const override { return static_cast<int>(CollisionType::Paddle); }

    void onCollision(ICollideable *, float, float) override {
    }
};

int main() {
    Display display(0, 1024, 768, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    TextManager textManager;
    if (!textManager.setFontTheme("../themes/default/font/fonts.txt")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
    }
    MockEventManager eventManager;
    MockBall mockBall;
    MockPaddle mockPaddle;

    const TextureManager textureManager;
    SpriteSheetAnimation tex;

    SpriteSheetAnimation texBorder;
    const std::filesystem::path borderTexPath = "../themes/default/gfx/border.png";
    const std::filesystem::path borderPropsPath = "../themes/default/gfx/border.txt";
    if (!textureManager.load(borderTexPath, texBorder)) {
        SDL_Log("Fehler beim Laden der Border-Textur: %s", borderTexPath.c_str());
        return EXIT_FAILURE;
    }
    textureManager.readTexProps(borderPropsPath, texBorder);

    // PlayfieldBorder-Objekte erzeugen
    PlayfieldBorder leftBorder(PlayfieldBorder::Side::Left, texBorder, &eventManager);
    PlayfieldBorder rightBorder(PlayfieldBorder::Side::Right, texBorder, &eventManager);
    PlayfieldBorder topBorder(PlayfieldBorder::Side::Top, texBorder, &eventManager);

    std::vector<std::string> instructions = {
        "1: Ball-Border-Top collision",
        "2: Ball-Border-Left collision",
        "3: Ball-Border-Right collision",
        "4: Paddle-Border-Left collision",
        "5: Paddle-Border-Right collision",
        "ESC: Quit"
    };

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
                } else if (event.key.keysym.sym == SDLK_1) {
                    // Setze den Kollisionspunkt auf der oberen Kante
                    float hitX = 0.0f;
                    float hitY = topBorder.getPosY();
                    topBorder.onCollision(&mockBall, hitX, hitY);
                } else if (event.key.keysym.sym == SDLK_2) {
                    float hitX = leftBorder.getPosX() + leftBorder.getWidth();
                    float hitY = 0.0f;
                    leftBorder.onCollision(&mockBall, hitX, hitY);
                } else if (event.key.keysym.sym == SDLK_3) {
                    float hitX = rightBorder.getPosX() - rightBorder.getWidth();
                    float hitY = 0.0f;
                    rightBorder.onCollision(&mockBall, hitX, hitY);
                } else if (event.key.keysym.sym == SDLK_4) {
                    float hitX = leftBorder.getPosX() + leftBorder.getWidth();
                    float hitY = mockPaddle.getPosY();
                    leftBorder.onCollision(&mockPaddle, hitX, hitY);
                } else if (event.key.keysym.sym == SDLK_5) {
                    float hitX = rightBorder.getPosX() - rightBorder.getWidth();
                    float hitY = mockPaddle.getPosY();
                    rightBorder.onCollision(&mockPaddle, hitX, hitY);
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spielfeldränder zeichnen
        leftBorder.draw(deltaTime);
        rightBorder.draw(deltaTime);
        topBorder.draw(deltaTime); // should not be visible

        float yPos = 0.9f;
        for (const auto &instruction: instructions) {
            textManager.write(instruction, FONT_MENU, true, 0.75f, -0.0f, yPos);
            yPos -= 0.07f;
        }
        SDL_GL_SwapWindow(display.sdlWindow);
        SDL_Delay(16); // ~60fps
    }
    return EXIT_SUCCESS;
}
