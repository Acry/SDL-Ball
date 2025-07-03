// MenuManager_Tests.cpp
#include <cstdlib>
#include <utility>

#include "CollisionManager.h"
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"

class TestMenuItem final : public ICollideable {
    float x, y, w, h;
    bool active;
    std::string label;
    GameEvent event;

public:
    TestMenuItem(float x, float y, float w, float h, std::string label, GameEvent event)
        : x(x), y(y), w(w), h(h), active(true), label(std::move(label)), event(event) {
    }

    [[nodiscard]] float getPosX() const override { return x; }
    [[nodiscard]] float getPosY() const override { return y; }
    [[nodiscard]] float getWidth() const override { return w; }
    [[nodiscard]] float getHeight() const override { return h; }
    [[nodiscard]] bool isActive() const override { return active; }
    void setActive(const bool value) override { active = value; }
    [[nodiscard]] CollisionType getCollisionType() const override { return CollisionType::None; }
    [[nodiscard]] const std::string &getLabel() const { return label; }
    [[nodiscard]] GameEvent getEvent() const { return event; }
};

struct MainMenu {
    static const std::string &title() {
        static const std::string t = "MainMenu";
        return t;
    }

    static std::vector<TestMenuItem> create() {
        return {
            TestMenuItem(-0.3f, 0.05f, 0.6, 0.08f, "New Game", GameEvent::LevelRequested),
            TestMenuItem(-0.3f, -0.05f, 0.6f, 0.08f, "Settings", GameEvent::SettingsLoadRequested),
            TestMenuItem(-0.3f, -0.15f, 0.6f, 0.08f, "Quit", GameEvent::QuitRequested)
        };
    }
};

class TestMenuManager {
    TextManager &textManager;
    IEventManager *eventManager;
    std::vector<TestMenuItem> items;
    bool visible = false;

public:
    TestMenuManager(TextManager &tm, IEventManager *evtMgr) : textManager(tm), eventManager(evtMgr) {
        for (const auto &item: MainMenu::create()) {
            addItem(item);
        }
        hoveredIndex = 0;
        eventManager->addListener(GameEvent::MenuKeyPressed, [this](const KeyboardEventData &) {
            if (this->isVisible()) {
                this->hide();
                const EventData data;
                eventManager->emit(GameEvent::MenuClosed, data);
                removeListeners();
            } else {
                this->show();
                const EventData data;
                eventManager->emit(GameEvent::MenuOpened, data);
                eventManager->addListener(GameEvent::MouseCoordinatesNormalized,
                                          [this](const MouseCoordinatesNormalizedEventData &data) {
                                              this->hoveredIndex = this->handleMouse(data.x, data.y);
                                          }, this);
                eventManager->addListener(GameEvent::MenuKeyReleased, [this](const KeyboardEventData &data) {
                    this->onKeyReleased(data);
                }, this);
                eventManager->addListener(GameEvent::MouseButtonReleasedNormalized, [this](const MouseEventData &data) {
                    if (!this->isVisible()) return;
                    if (data.button != SDL_BUTTON_LEFT) return; // Nur linker Mausklick
                    int idx = this->handleMouse(data.x, data.y);
                    if (idx >= 0 && idx < static_cast<int>(items.size())) {
                        hoveredIndex = idx;
                        const auto &item = items[hoveredIndex];
                        SDL_Log("Menu item clicked: %d", hoveredIndex);
                        EventData eventData;
                        eventManager->emit(item.getEvent(), eventData);
                    }
                }, this);
            }
        }, this);
    }

    void removeListeners() {
        eventManager->removeListener(GameEvent::MouseCoordinatesNormalized, this);
        eventManager->removeListener(GameEvent::MenuKeyReleased, this);
        eventManager->removeListener(GameEvent::MouseButtonReleasedNormalized, this);
    }

    ~TestMenuManager() {
        eventManager->removeListener(GameEvent::MenuKeyPressed, this);
        removeListeners();
    }

    void show() { visible = true; }
    void hide() { visible = false; }
    [[nodiscard]] bool isVisible() const { return visible; }
    void addItem(const TestMenuItem &item) { items.push_back(item); }
    int hoveredIndex = -1;

    int handleMouse(const float mx, const float my) const {
        for (size_t i = 0; i < items.size(); ++i) {
            if (CollisionManager::pointInsideRect(items[i], mx, my)) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void onKeyReleased(const KeyboardEventData &data) {
        if (!(data.key == SDLK_RETURN || data.key == SDLK_KP_ENTER ||
              data.key == SDLK_UP || data.key == SDLK_DOWN ||
              data.key == SDLK_LEFT || data.key == SDLK_RIGHT)) {
            return;
        }
        if (data.key == SDLK_UP) {
            if (hoveredIndex > 0) hoveredIndex--;
            else hoveredIndex = static_cast<int>(items.size()) - 1;
            return;
        }
        if (data.key == SDLK_DOWN) {
            if (hoveredIndex < static_cast<int>(items.size()) - 1) hoveredIndex++;
            else hoveredIndex = 0;
            return;
        }
        if (data.key == SDLK_RETURN || data.key == SDLK_KP_ENTER) {
            SDL_Log("Menu item selected: %d", hoveredIndex);
            if (hoveredIndex >= 0 && hoveredIndex < static_cast<int>(items.size())) {
                const auto &item = items[hoveredIndex];
                SDL_Log("Selected event: %d", static_cast<int>(item.getEvent()));
                EventData eventData;
                eventManager->emit(item.getEvent(), eventData);
            }
        }
    }

    void render() const {
        drawMenuBackground();
        const GLfloat fontHeight = textManager.getHeight(Fonts::IntroHighscore);
        textManager.write(
            MainMenu::title(),
            Fonts::IntroHighscore,
            true,
            1.0f,
            menuX + menuW / 2.0f,
            menuY + menuH - fontHeight * 0.2f
        );
        for (size_t i = 0; i < items.size(); ++i) {
            if (static_cast<int>(i) == hoveredIndex) {
                drawHoveredItem(items[i]);
            } else {
                drawItem(items[i]);
            }
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

        const GLfloat fontHeight = textManager.getHeight(Fonts::Menu);

        // Vertikal zentrieren: Mittelpunkt der Box minus halbe Texthöhe
        const float textY = item.getPosY() + item.getHeight() / 2.0f - fontHeight / 2.0f;

        textManager.write(
            item.getLabel(),
            Fonts::Menu,
            true,
            1.0f,
            item.getPosX() + item.getWidth() / 2.0f,
            item.getPosY() + fontHeight
        );
    }

    void drawHoveredItem(const TestMenuItem &item) const {
        // Highlight-Farbe
        glColor3f(0.8f, 0.5f, 0.2f);
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

        const GLfloat fontHeight = textManager.getHeight(Fonts::Menu);
        textManager.write(
            item.getLabel(),
            Fonts::Menu,
            true,
            1.0f,
            item.getPosX() + item.getWidth() / 2.0f,
            item.getPosY() + fontHeight
        );
    }

    void drawMenuBackground() const {
        // Abdunkelung (Fullscreen-Overlay)
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f); // Schwarz, 40% Deckkraft
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();

        // Fill
        glColor4f(0.15f, 0.15f, 0.15f, 0.8f);
        drawRoundedRect(menuX, menuY, menuW, menuH);
        // glBegin(GL_QUADS);
        // glVertex2f(menuX, menuY);
        // glVertex2f(menuX + menuW, menuY);
        // glVertex2f(menuX + menuW, menuY + menuH);
        // glVertex2f(menuX, menuY + menuH);
        // glEnd();
        //
        //
        // // Border
        // glColor3f(1.0f, 1.0f, 1.0f);
        // glLineWidth(2.0f);
        // glBegin(GL_LINE_LOOP);
        // glVertex2f(menuX, menuY);
        // glVertex2f(menuX + menuW, menuY);
        // glVertex2f(menuX + menuW, menuY + menuH);
        // glVertex2f(menuX, menuY + menuH);
        // glEnd();
        glDisable(GL_BLEND);
    }

    // Hilfsfunktion zum Zeichnen eines abgerundeten Rechtecks
    void drawRoundedRect(float x, float y, float w, float h, float radius = 0.1f, int segments = 16,
                         float r = 0.3f, float g = 0.3f, float b = 0.3f, bool drawBorder = true) const {
        // Füllfarbe setzen
        // glColor3f(r, g, b);
        glColor4f(0.15f, 0.15f, 0.15f, 0.8f);


        // Ecken (Viertelkreise) in gleicher Farbe
        float cx[4] = {x + radius, x + w - radius, x + w - radius, x + radius};
        float cy[4] = {y + radius, y + radius, y + h - radius, y + h - radius};
        float startAngle[4] = {M_PI, 1.5f * M_PI, 0.0f, 0.5f * M_PI};

        for (int i = 0; i < 4; ++i) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx[i], cy[i]);
            for (int j = 0; j <= segments; ++j) {
                float angle = startAngle[i] + (j / (float) segments) * (M_PI / 2.0f);
                glVertex2f(cx[i] + cosf(angle) * radius, cy[i] + sinf(angle) * radius);
            }
            glEnd();
        }

        // Dann die vier Kanten als Rechtecke
        glBegin(GL_QUADS);
        // Oben
        glVertex2f(x + radius, y);
        glVertex2f(x + w - radius, y);
        glVertex2f(x + w - radius, y + radius);
        glVertex2f(x + radius, y + radius);
        // Unten
        glVertex2f(x + radius, y + h - radius);
        glVertex2f(x + w - radius, y + h - radius);
        glVertex2f(x + w - radius, y + h);
        glVertex2f(x + radius, y + h);
        // Links
        glVertex2f(x, y + radius);
        glVertex2f(x + radius, y + radius);
        glVertex2f(x + radius, y + h - radius);
        glVertex2f(x, y + h - radius);
        // Rechts
        glVertex2f(x + w - radius, y + radius);
        glVertex2f(x + w, y + radius);
        glVertex2f(x + w, y + h - radius);
        glVertex2f(x + w - radius, y + h - radius);
        glEnd();

        // Zuletzt die zentrale Fläche
        glBegin(GL_QUADS);
        glVertex2f(x + radius, y + radius);
        glVertex2f(x + w - radius, y + radius);
        glVertex2f(x + w - radius, y + h - radius);
        glVertex2f(x + radius, y + h - radius);
        glEnd();

        // Optional: Weißer Rahmen
        if (drawBorder) {
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j <= segments; ++j) {
                    float angle = startAngle[i] + (j / (float) segments) * (M_PI / 2.0f);
                    glVertex2f(cx[i] + cosf(angle) * radius, cy[i] + sinf(angle) * radius);
                }
            }
            glEnd();
        }
    }

private:
    const float menuX = -0.4f;
    const float menuY = -0.5f;
    const float menuW = 0.8f;
    const float menuH = 1.0f;
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
          textManager(&eventManager),
          collisionManager(&eventManager),
          menuManager(textManager, &eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: MenuManager Test");
        textManager.setTheme("../tests/themes/test");
    }
};

class MenuManagerTestHelper final : public TestHelper {
    MenuManagerTestContext &ctx;

public:
    explicit MenuManagerTestHelper(MenuManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (ctx.menuManager.isVisible()) {
            ctx.menuManager.render();
        }
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        MenuManagerTestContext ctx;
        EventDispatcher eventDispatcher(&ctx.eventManager);
        MenuManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "M: Draw Mouse Coordinates",
            "TAB: Toggle Menu",
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
