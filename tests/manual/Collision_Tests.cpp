// Collision_Tests.cpp
#include <cstdlib>

#include "DisplayManager.hpp"
#include "CollisionManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"

class SimpleCollideable : public ICollideable {
    float x, y, w, h;
    bool active;
    CollisionType type;

public:
    SimpleCollideable(float x, float y, float w, float h, CollisionType type)
        : x(x), y(y), w(w), h(h), active(true), type(type) {
    }

    float getPosX() const override { return x; }
    float getPosY() const override { return y; }
    float getWidth() const override { return w; }
    float getHeight() const override { return h; }
    bool isActive() const override { return active; }
    void setActive(bool value) override { active = value; }
    CollisionType getCollisionType() const override { return type; }

    void setPosition(float nx, float ny) {
        x = nx;
        y = ny;
    }
};


class CollisionManagerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    CollisionManager collisionManager;

    CollisionManagerTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          collisionManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: CollisionManager Test");
        textManager.setTheme("../tests/themes/test");
    }
};

class CollisionManagerTestHelper final : public TestHelper {
    CollisionManagerTestContext &ctx;
    SimpleCollideable obj1;
    SimpleCollideable obj2;
    bool dragging1 = false, dragging2 = false;
    float dragOffsetX = 0, dragOffsetY = 0;

public:
    explicit CollisionManagerTestHelper(CollisionManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context),
          obj1(-0.5f, -0.5f, 0.3f, 0.3f, CollisionType::Ball),
          obj2(0.2f, 0.2f, 0.3f, 0.3f, CollisionType::Paddle) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
            SDL_Log("M (%.2f, %.2f)", m_mouseX, m_mouseY);
            if (ctx.collisionManager.pointInsideRect(obj1, m_mouseX, m_mouseY)) {
                dragging1 = !dragging1;
                dragOffsetX = m_mouseX - obj1.getPosX();
                dragOffsetY = m_mouseY - obj1.getPosY();
                SDL_Log("Collide obj1 at (%.2f, %.2f)", m_mouseX, m_mouseY);
            } else if (ctx.collisionManager.pointInsideRect(obj2, m_mouseX, m_mouseY)) {
                dragging2 = !dragging2;
                dragOffsetX = m_mouseX - obj2.getPosX();
                dragOffsetY = m_mouseY - obj2.getPosY();
                SDL_Log("Collide obj2 at (%.2f, %.2f)", m_mouseX, m_mouseY);
            }
        }
    }

    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) override {
        TestHelper::handleMouseCoordinatesNormalized(data);
        if (dragging1) {
            obj1.setPosition(data.x - dragOffsetX, data.y - dragOffsetY);
        } else if (dragging2) {
            obj2.setPosition(data.x - dragOffsetX, data.y - dragOffsetY);
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();

        drawRect(obj1.getPosX(), obj1.getPosY(), obj1.getWidth(), obj1.getHeight(), 1.0f, 0.0f, 0.0f);
        drawRect(obj2.getPosX(), obj2.getPosY(), obj2.getWidth(), obj2.getHeight(), 0.0f, 0.0f, 1.0f);
        //bool collided = ctx.collisionManager.checkCollision(obj1, obj2);
        //if (collided) {
        //    SDL_Log("Collision detected between obj1 and obj2");
        //    float hitX, hitY;
        //    ctx.collisionManager.checkCollision(obj1, obj2, hitX, hitY);
        //    SDL_Log("Collision point: (%f, %f)", hitX, hitY);
        //}
        drawCenterLines();
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }

private:
    void drawRect(float x, float y, float w, float h, float r, float g, float b) {
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
        CollisionManagerTestContext ctx;
        const EventDispatcher eventDispatcher(&ctx.eventManager);
        CollisionManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
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
