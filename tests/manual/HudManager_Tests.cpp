// HudManager_Tests.cpp

/* todo:
 * score via event
 * toggle clock via event (MenuManager)
 * add speedometer component
 * then gameManager
 * add shop component, after powerup manager
 */

#include <cstdlib>
#include <vector>
#include <memory>
#include <chrono>

#include "Clock.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "Score.h"
#include "Lifes.h"

class HudManager {
    IEventManager *eventManager;
    TextManager *textManager;
    TextureManager *textureManager;
    Score score;
    Clock clock;
    Lifes lifes;

    bool clockEnabled = true;

public:
    HudManager(IEventManager *eventManager, TextManager *textManager, TextureManager *textureManager)
        : eventManager(eventManager),
          textManager(textManager),
          textureManager(textureManager),
          score(*textManager),
          clock(*textManager),
          lifes(textureManager, 21) {
    }

    void addPoints(const int points) {
        score.addPoints(points);
    }

    void update(const float deltaTime) {
        score.update(deltaTime);
        if (clockEnabled) {
            clock.update(deltaTime);
        }
    }

    void draw() const {
        score.draw();
        lifes.draw();
        if (clockEnabled) {
            clock.draw();
        }
    }

    void resetScore() {
        score.reset();
    }

    void toggleClock() {
        clockEnabled = !clockEnabled;
    }
};

class HudManagerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<HudManager> hudManager;

    HudManagerTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: HudManager Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        textureManager->setSpriteTheme("../themes/default");
        hudManager = std::make_unique<HudManager>(&eventManager, &textManager, textureManager.get());
    }
};

class HudManagerTestHelper final : public TestHelper {
    HudManagerTestContext &ctx;

public:
    explicit HudManagerTestHelper(HudManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);

        switch (data.key) {
            case SDLK_1:
                ctx.hudManager->addPoints(500);
                break;
            case SDLK_2:
                ctx.hudManager->addPoints(-500);
                break;
            case SDLK_c:
                ctx.hudManager->toggleClock();
                break;
            case SDLK_r:
                ctx.hudManager->resetScore();
                break;
            default:
                break;
        }
    }

    void update(const float deltaTime) const {
        ctx.hudManager->update(deltaTime);
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();

        ctx.hudManager->draw();

        renderInstructions(deltaTime, instructions);

        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }

private:
    static void renderFullscreenTexture(const GLuint textureId) {
        if (textureId <= 0) return;

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }
};

int main() {
    try {
        HudManagerTestContext ctx;
        EventDispatcher eventDispatcher(&ctx.eventManager);
        HudManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "ESC: Quit",
            "1: add 500 points",
            "2: subtract 500 points",
            "+/-: add/subtract lives",
            "C: Toggle Clock",
            "R: Reset Score",
            "M: Toggle Mouse Coordinates",
            "S: Screenshot"
        };

        bool running = true;
        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            running = eventDispatcher.processEvents();

            testHelper.update(deltaTime);
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
