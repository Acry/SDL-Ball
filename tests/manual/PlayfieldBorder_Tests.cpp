// PlayfieldBorder_Tests.cpp
#include <cstdlib>

#include "DisplayManager.hpp"
#include "CollisionManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "PlayfieldBorder.h"


class BorderTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    CollisionManager collisionManager;
    std::unique_ptr<PlayfieldBorder> leftBorder;
    std::unique_ptr<PlayfieldBorder> rightBorder;
    std::unique_ptr<PlayfieldBorder> topBorder;

    BorderTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          collisionManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Border Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
        auto *borderTex = textureManager->getMiscTexture(MiscTexture::Border);
        leftBorder = std::make_unique<PlayfieldBorder>(PlayfieldBorder::Side::Left, *borderTex, &eventManager);
        rightBorder = std::make_unique<PlayfieldBorder>(PlayfieldBorder::Side::Right, *borderTex, &eventManager);
        topBorder = std::make_unique<PlayfieldBorder>(PlayfieldBorder::Side::Top, *borderTex, &eventManager);
    }
};

class BorderTestHelper final : public TestHelper {
    BorderTestContext &ctx;

public:
    explicit BorderTestHelper(BorderTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_1: {
                const LevelData testLevelDataDropping{
                    .themeName = "",
                    .dropSpeed = 5000,
                };
                ctx.eventManager.emit(GameEvent::LevelLoaded, testLevelDataDropping);
            }
            break;
            case SDLK_2: {
                const LevelData testLevelData{
                    .themeName = "",
                    .dropSpeed = 0,
                };
                ctx.eventManager.emit(GameEvent::LevelLoaded, testLevelData);
            }
            break;
            default:
                break;
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
        }
    }

    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) override {
        TestHelper::handleMouseCoordinatesNormalized(data);
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ctx.leftBorder->draw();
        ctx.rightBorder->draw();
        ctx.topBorder->update(deltaTime);
        ctx.topBorder->draw();
        drawGrid();
        drawCenterLines();
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }

private:
    static void drawRect(const float x, const float y, const float w, const float h, const float r, const float g,
                         const float b) {
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
        glEnd();
    }
};

int main() {
    try {
        BorderTestContext ctx;
        const EventDispatcher eventDispatcher(&ctx.eventManager);
        BorderTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "1: Emit Dropping Level",
            "2: Emit Static Level",
            "M: Draw Mouse Coordinates",
            "S: Screenshot",
            "ESC: Beenden"
        };

        bool running = true;
        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            running = eventDispatcher.processEvents();
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
