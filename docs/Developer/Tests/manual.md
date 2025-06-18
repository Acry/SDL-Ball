# Manual Tests-Code

`../../../tests/manual`

## Currently implemented manual tests

- test-backgroundManager
- test-ballManager (test-driven)
- test-brickManager (try test-setup: class?)
- test-codeManager (stub, early setup)
- test-collisionManager (needs GameObjectManagers)
- test-configFileManager
- test-displayManager
- test-effectManager (need texture changes, needs new test-setup)
- test-levelManager (uses test-setup: context)
- test-paddle (needs: manager and class rework)
- test-settingsManager
- test-soundManager
- test-spriteSheetAnimation
- test-spriteSheetAnimationManager
- test-textManager
- test-textureManager
- test-themeManager

## Currently Working manual tests

Before I fix all currently implemented Tests, I want to choose a final test-setup, so I can use it for all tests.

- test-backgroundManager (uses test-setup: MyTestHelper)
- test-ballManager (test-driven)
- test-brickManager (try test-setup: class?)
- test-codeManager (stub, early setup)
- test-configFileManager
- test-displayManager
- test-levelManager (uses test-setup: context)
- test-settingsManager
- test-soundManager
- test-spriteSheetAnimation
- test-spriteSheetAnimationManager
- test-textManager
- test-textureManager
- test-themeManager

## Planned manual tests (WIP)

- test-backgroundManager
- test-ball
- test-ballManager
- test-brick
- test-brickManager
- test-codeManager
- test-collisionManager
- test-configFileManager
- test-displayManager
- test-effectManager
- test-eventDispatcher
- test-eventManager
- test-Hud
- test-hudManager
- test-levelManager
- test-MenuManager
- test-mouseManager
- test-paddle
- test-paddleManager (PlayerManager)
- test-Powerup
- test-powerupManager
- test-sameGame
- test-saveGameManager
- test-sceneManager
- test-settingsManager
- test-soundManager
- test-spriteSheetAnimation
- test-spriteSheetAnimationManager
- test-textManager
- test-textureManager
- test-themeManager

## Test-Setup

### Global State and virtual handleKeyPress

I do not like this attempt to use global variables

```c++
LevelManager *levelManager = nullptr;
BackgroundManager *backgroundManager = nullptr;
TextureManager *textureManager = nullptr;
TextManager *textManager = nullptr;
size_t currentLevel = 1;
size_t levelCount = 0;

class MyTestHelper final : public TestHelper {
public:
using TestHelper::TestHelper;

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data); // Basisverhalten
        switch (data.key) {
            case SDLK_o:
                backgroundManager->setBackgroundOverlayEnabled(!backgroundManager->isBackgroundOverlayEnabled());
                break;
            case SDLK_t: {
                static bool useDefaultTheme = false;
                const std::filesystem::path newTheme = useDefaultTheme
                                                           ? "../themes/default"
                                                           : "../tests/themes/test";

                if (!textureManager->setBackgroundTheme(newTheme)) {
                    textManager->addAnnouncement("Error changing theme", 1500, Fonts::AnnounceBad);
                } else {
                    textManager->addAnnouncement("Theme changed", 1500, Fonts::AnnounceGood);
                    // levelManager.loadLevel(currentLevel);
                }
                useDefaultTheme = !useDefaultTheme;
                break;
            }
            case SDLK_SPACE:
                currentLevel++;
                if (currentLevel > levelCount) currentLevel = 1; {
                    levelManager->loadLevel(currentLevel);
                }
                break;

            // ...
            default: ;
        }
    }
};
```

### TestContext

```c++
struct TestContext {
    LevelManager levelManager;
    BackgroundManager backgroundManager;
    TextureManager textureManager;
    TextManager textManager;
    size_t currentLevel = 1;
    size_t levelCount = 0;

    TestContext(EventManager& eventManager, const std::filesystem::path& themePath)
        : levelManager(&eventManager),
          textureManager(),
          backgroundManager(textureManager),
          textManager()
    {
        textManager.setTheme(themePath);
        textureManager.setBackgroundTheme(themePath);
        backgroundManager.registerEvents(&eventManager);
        levelManager.setTheme(themePath);
        levelCount = levelManager.getLevelCount();
        levelManager.loadLevel(currentLevel);
    }
};

int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) return EXIT_FAILURE;
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Background Test");

    const std::filesystem::path themePath = "../themes/default";
    TestContext ctx(eventManager, themePath);

    // Übergib ctx an deine Helper oder nutze Referenzen darauf
    // Beispiel: MyTestHelper testHelper(ctx.textManager, &eventManager, ctx);
}
```

### TestClass

```c++
// TestContext.h
#pragma once
#include "LevelManager.h"
#include "BackgroundManager.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "EventManager.h"

struct TestContext {
    LevelManager levelManager;
    BackgroundManager backgroundManager;
    TextureManager textureManager;
    TextManager textManager;
    size_t currentLevel = 1;
    size_t levelCount = 0;

    TestContext(EventManager& eventManager, const std::filesystem::path& themePath)
        : levelManager(&eventManager),
          textureManager(),
          backgroundManager(textureManager),
          textManager()
    {
        textManager.setTheme(themePath);
        textureManager.setBackgroundTheme(themePath);
        backgroundManager.registerEvents(&eventManager);
        levelManager.setTheme(themePath);
        levelCount = levelManager.getLevelCount();
        levelManager.loadLevel(currentLevel);
    }
};
```

```c++
// MyTestHelper.h
#pragma once
#include "TestHelper.h"
#include "TestContext.h"

class MyTestHelper : public TestHelper {
    TestContext& ctx;
public:
    MyTestHelper(TextManager& textManager, IEventManager* eventManager, TestContext& context)
        : TestHelper(textManager, eventManager), ctx(context) {}

    void handleKeyPress(const KeyboardEventData& data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_o:
                ctx.backgroundManager.setBackgroundOverlayEnabled(!ctx.backgroundManager.isBackgroundOverlayEnabled());
                break;
            case SDLK_t: {
                static bool useDefaultTheme = false;
                const std::filesystem::path newTheme = useDefaultTheme
                    ? "../themes/default"
                    : "../tests/themes/test";
                if (!ctx.textureManager.setBackgroundTheme(newTheme)) {
                    ctx.textManager.addAnnouncement("Error changing theme", 1500, Fonts::AnnounceBad);
                } else {
                    ctx.textManager.addAnnouncement("Theme changed", 1500, Fonts::AnnounceGood);
                }
                useDefaultTheme = !useDefaultTheme;
                break;
            }
            case SDLK_SPACE:
                ctx.currentLevel++;
                if (ctx.currentLevel > ctx.levelCount) ctx.currentLevel = 1;
                ctx.levelManager.loadLevel(ctx.currentLevel);
                break;
            case SDLK_LEFT:
                ctx.currentLevel--;
                if (ctx.currentLevel < 1) ctx.currentLevel = ctx.levelCount;
                ctx.levelManager.loadLevel(ctx.currentLevel);
                break;
            // Weitere Tasten...
            default: break;
        }
    }
};
```

```c++
#include "TestContext.h"
#include "MyTestHelper.h"

int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) return EXIT_FAILURE;
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Background Test");

    const std::filesystem::path themePath = "../themes/default";
    TestContext ctx(eventManager, themePath);

    const EventDispatcher eventDispatcher(&eventManager);
    MyTestHelper testHelper(ctx.textManager, &eventManager, ctx);

    const std::vector<std::string> instructions = {
        "Rendering Background based on level",
        "M: Toggle Mouse Coordinates",
        "O: Toggle Background Overlay",
        "S: Create Screenshot",
        "T: Toggle Theme",
        "-> - next level",
        "<- - previous level",
        "Pos1 - first level",
        "End - last level",
        "ESC: Quit"
    };

    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    bool running = true;
    while (running) {
        running = eventDispatcher.processEvents();
```

### TestClass driven further

```c++
int main() {
    BackgroundTest backgroundTest;
    if (!backgroundTest.init()) return EXIT_FAILURE;
    while (backgroundTest.run());
    return EXIT_SUCCESS;
}
```

```c++
class BackgroundTest {
TestContext ctx;
DisplayManager displayManager;
EventManager eventManager;
MouseManager mouseManager;
EventDispatcher eventDispatcher;
MyTestHelper testHelper;
std::vector<std::string> instructions;
Uint32 lastTime;
float deltaTime;
bool running;

public:
    BackgroundTest()
        : ctx(eventManager, "../themes/default"),
          displayManager(&eventManager),
          mouseManager(&eventManager),
          eventDispatcher(&eventManager),
          testHelper(ctx.textManager, &eventManager, ctx),
          instructions({
              "Rendering Background based on level",
              "M: Toggle Mouse Coordinates",
              "O: Toggle Background Overlay",
              "S: Create Screenshot",
              "T: Toggle Theme",
              "-> - next level",
              "<- - previous level",
              "Pos1 - first level",
              "End - last level",
              "ESC: Quit"
          }),
          lastTime(0),
          deltaTime(0.0f),
          running(true)
    {}

    bool init() {
        if (!displayManager.init(0, 1024, 768, false)) return false;
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Background Test");
        lastTime = SDL_GetTicks();
        return true;
    }

    bool run() {
        if (!running) return false;
        running = eventDispatcher.processEvents();
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ctx.backgroundManager.draw();
        testHelper.renderInstructions(deltaTime, instructions);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        char tempTexto[64];
        sprintf(tempTexto, "Overlay: %s", ctx.backgroundManager.isBackgroundOverlayEnabled() ? "ON" : "OFF");
        ctx.textManager.write(tempTexto, Fonts::Highscore, true, 0.5f, 0.0f, -0.4f);

        char tempText[64];
        sprintf(tempText, "Level: %lu", ctx.currentLevel);
        ctx.textManager.write(tempText, Fonts::Highscore, true, 1.0f, 0.0f, -0.5f);
        testHelper.drawMouseCoordinates();

        SDL_GL_SwapWindow(displayManager.sdlWindow);
        return running;
    }
};
```

## See also

[Managers.md](../Managers.md)
