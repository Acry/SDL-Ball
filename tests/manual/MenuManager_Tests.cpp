// MenuManager_Tests.cpp
#include <cstdlib>

#include "DisplayManager.hpp"
#include "CollisionManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"


class TestMenuItem final : public ICollideable {
    float x, y, w, h;
    bool active;
    std::string label;

public:
    TestMenuItem(float x, float y, float w, float h, const std::string &label)
        : x(x), y(y), w(w), h(h), active(true), label(label) {
    }

    float getPosX() const override { return x; }
    float getPosY() const override { return y; }
    float getWidth() const override { return w; }
    float getHeight() const override { return h; }
    bool isActive() const override { return active; }
    void setActive(bool value) override { active = value; }
    CollisionType getCollisionType() const override { return CollisionType::None; }
    const std::string &getLabel() const { return label; }
};

class TestMenuManager {
    TextManager &textManager;
    std::vector<TestMenuItem> items;
    bool visible = false;

public:
    TestMenuManager(TextManager &tm) : textManager(tm) {
    }

    void show() { visible = true; }
    void hide() { visible = false; }
    bool isVisible() const { return visible; }
    void addItem(const TestMenuItem &item) { items.push_back(item); }

    int handleMouse(float mx, float my) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (CollisionManager::pointInsideRect(items[i], mx, my)) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void render() const {
        drawMenuBackground();
        for (const auto &item: items) {
            drawItem(item);
        }
    }

    void drawItem(const TestMenuItem &item) const {
        // Füllung
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(item.getPosX(), item.getPosY());
        glVertex2f(item.getPosX() + item.getWidth(), item.getPosY());
        glVertex2f(item.getPosX() + item.getWidth(), item.getPosY() + item.getHeight());
        glVertex2f(item.getPosX(), item.getPosY() + item.getHeight());
        glEnd();

        // Rahmen
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(item.getPosX(), item.getPosY());
        glVertex2f(item.getPosX() + item.getWidth(), item.getPosY());
        glVertex2f(item.getPosX() + item.getWidth(), item.getPosY() + item.getHeight());
        glVertex2f(item.getPosX(), item.getPosY() + item.getHeight());
        glEnd();

        textManager.write(item.getLabel(), Fonts::Menu, true, 1.0f,
                          item.getPosX() + item.getWidth() / 2.0f,
                          item.getPosY() + item.getHeight() / 2.0f);
    }

    void drawMenuBackground() const {
        // Fill
        glColor3f(0.15f, 0.15f, 0.15f);
        glBegin(GL_QUADS);
        glVertex2f(menuX, menuY);
        glVertex2f(menuX + menuW, menuY);
        glVertex2f(menuX + menuW, menuY + menuH);
        glVertex2f(menuX, menuY + menuH);
        glEnd();

        // Border
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(menuX, menuY);
        glVertex2f(menuX + menuW, menuY);
        glVertex2f(menuX + menuW, menuY + menuH);
        glVertex2f(menuX, menuY + menuH);
        glEnd();
    }

private:
    const float menuX = -0.25f;
    const float menuY = -0.15f;
    const float menuW = 0.5f;
    const float menuH = 0.3f;
};

class MenuManagerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    CollisionManager collisionManager;
    TestMenuManager menuManager;

    MenuManagerTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          collisionManager(&eventManager),
          menuManager(textManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: CollisionManager Test");
        textManager.setTheme("../tests/themes/test");
    }
};

class MenuManagerTestHelper final : public TestHelper {
    MenuManagerTestContext &ctx;
    bool dragging1 = false, dragging2 = false;
    float dragOffsetX = 0, dragOffsetY = 0;

public:
    explicit MenuManagerTestHelper(MenuManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        if (data.key == SDLK_o) {
            if (!ctx.menuManager.isVisible()) {
                ctx.menuManager.addItem(TestMenuItem(-0.2f, -0.1f, 0.4f, 0.2f, "Quit"));
                ctx.menuManager.show();
            } else {
                ctx.menuManager.hide();
            }
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
        if (ctx.menuManager.isVisible()) {
            ctx.menuManager.render();
        }
        drawGrid();
        drawCenterLines();
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        MenuManagerTestContext ctx;
        const EventDispatcher eventDispatcher(&ctx.eventManager);
        MenuManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "M: Draw Mouse Coordinates",
            "O: Toggle Menu",
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
