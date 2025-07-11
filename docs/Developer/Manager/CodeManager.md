# CodeManager

The code manager holds the timing and flow of the program.

Der CodeManager instanziiert alle anderen Manager und
steuert den Ablauf. Er spricht direkt mit dem SettingsManager, dem Theme-Manager und dem configfileManager.
Der [EventManager](EventManager.md) ist die zentrale Kommunikationsschnittstelle.

```text
SettingsManager → ThemeManager → TextureManager → SceneManager
       ↑               ↑             ↑
       └───────────────┴─────────────┘
              ConfigFileManager
                 CodeManager
```

Der GameManager startet die GameLoop, wenn der CodeManager das Event `GameStart` auslöst.

```c++
int main() {
    CodeManager codeManager;
    codeManager.initialize();
    codeManager.run();
    codeManager.shutdown();
    return EXIT_SUCCESS;
}
```

```c++

public:
CodeManager() : isRunning(false) {}

    // Initialize all systems
    void initialize() {
        sceneManager = std::make_unique<SceneManager>();
        eventManager = std::make_unique<EventManager>();
        gameObjectManager = std::make_unique<GameObjectManager>();

        // Add example scenes (e.g., MainMenu, Gameplay)
        sceneManager->addScene("MainMenu", std::make_unique<MainMenuScene>());
        sceneManager->addScene("Gameplay", std::make_unique<GameplayScene>());

        // Set initial scene
        sceneManager->setActiveScene("MainMenu");

        isRunning = true;
    }

    void run() {
        using namespace std::chrono;
        auto lastTime = high_resolution_clock::now();

        while (isRunning) {
            // Calculate deltaTime
            auto currentTime = high_resolution_clock::now();
            float deltaTime = duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            // Handle events (e.g., input)
            eventManager->processEvents(); // May use deltaTime for timed events

            // Update game logic
            sceneManager->update(deltaTime); // Updates active scene and its game objects
            gameObjectManager->update(deltaTime); // If objects are managed outside scenes

            // Render
            sceneManager->render();

            // Optional: Cap frame rate (e.g., sleep to target 60 FPS)
            // Add logic here if needed
        }
    }

    // Shutdown
    void shutdown() {
        isRunning = false;
        sceneManager->clearScenes();
        // Cleanup other managers as needed
    }

private:
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<EventManager> eventManager;
    std::unique_ptr<GameObjectManager> gameObjectManager;
    bool isRunning;
};

void run() {
using namespace std::chrono;
auto lastTime = high_resolution_clock::now();
const float targetFrameTime = 1.0f / 60.0f; // 60 FPS

    while (isRunning) {
        auto currentTime = high_resolution_clock::now();
        float deltaTime = duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        eventManager->processEvents();
        sceneManager->update(deltaTime);
        gameObjectManager->update(deltaTime);
        sceneManager->render();

        // Cap frame rate
        auto frameEnd = high_resolution_clock::now();
        float frameTime = duration<float>(frameEnd - currentTime).count();
        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(duration<float>(targetFrameTime - frameTime));
        }
    }

}
```

## Option

```c++
// CodeManager.h
#pragma once
#include "IEventManager.h"
#include "ConfigFileManager.h"
#include "SettingsManager.h"
#include "ThemeManager.h"
#include "DisplayManager.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "SceneManager.h"
#include "GameManager.h"

class CodeManager {
private:
// Pointer für alle Manager
std::unique_ptr<IEventManager> eventManager;
std::unique_ptr<ConfigFileManager> configFileManager;
std::unique_ptr<SettingsManager> settingsManager;
std::unique_ptr<ThemeManager> themeManager;
std::unique_ptr<DisplayManager> displayManager;
std::unique_ptr<TextureManager> textureManager;
std::unique_ptr<TextManager> textManager;
std::unique_ptr<SceneManager> sceneManager;
std::unique_ptr<GameManager> gameManager;

    bool isInitialized = false;
    bool isRunning = false;

    void initEventListeners();
    void onLevelThemeRequested(const ThemeData &data);
    void applyTheme(const std::string &themeName);

public:
CodeManager();
~CodeManager();

    bool initialize();
    void run();
    void shutdown();

};
```

```c++

#include "CodeManager.h"

CodeManager::CodeManager() = default;

CodeManager::~CodeManager() {
shutdown();
}

bool CodeManager::initialize() {
if (isInitialized) return true;

    // Alle Manager in der richtigen Reihenfolge erstellen
    eventManager = std::make_unique<EventManager>();
    configFileManager = std::make_unique<ConfigFileManager>();
    settingsManager = std::make_unique<SettingsManager>();
    displayManager = std::make_unique<DisplayManager>();
    textureManager = std::make_unique<TextureManager>();
    textManager = std::make_unique<TextManager>();
    themeManager = std::make_unique<ThemeManager>();
    sceneManager = std::make_unique<SceneManager>();
    gameManager = std::make_unique<GameManager>();

    // Manager initialisieren
    if (!eventManager->init() || 
        !configFileManager->init() ||
        !settingsManager->init() ||
        !displayManager->init() ||
        !textureManager->init() ||
        !textManager->init() ||
        !themeManager->init() ||
        !sceneManager->init() ||
        !gameManager->init()) {
        return false;
    }

    initEventListeners();
    isInitialized = true;
    return true;

}

void CodeManager::run() {
if (!isInitialized) return;

    isRunning = true;
    while (isRunning) {
        // Event-Verarbeitung
        eventManager->processEvents();
        
        // Update-Logik
        gameManager->update();
        sceneManager->update();
        
        // Rendering
        displayManager->clear();
        sceneManager->render();
        displayManager->swap();
        
        // Prüfen, ob das Programm beendet werden soll
        if (eventManager->shouldQuit()) {
            isRunning = false;
        }
    }

}

void CodeManager::shutdown() {
if (!isInitialized) return;

    isRunning = false;
    
    // Manager in umgekehrter Reihenfolge herunterfahren
    gameManager.reset();
    sceneManager.reset();
    themeManager.reset();
    textManager.reset();
    textureManager.reset();
    displayManager.reset();
    settingsManager.reset();
    configFileManager.reset();
    eventManager.reset();
    
    isInitialized = false;

}

void CodeManager::initEventListeners() {
// Event-Listener registrieren
// z.B.: eventManager->addListener(EventType::THEME_REQUESTED,
// std::bind(&CodeManager::onLevelThemeRequested, this, std::placeholders::_1));
}

void CodeManager::onLevelThemeRequested(const ThemeData &data) {
applyTheme(data.themeName);
}

void CodeManager::applyTheme(const std::string &themeName) {
themeManager->applyTheme(themeName);
}
```

## sketching SplashScene

```c++

codeManager.initSplashScene()

// CodeManager.h
#pragma once
#include <thread>
#include <atomic>
#include "SplashScreen.h"

class CodeManager {
private:
std::unique_ptr<SplashScreen> splashScreen;
std::atomic<bool> isInitialized;
std::atomic<bool> splashSceneRunning;
std::thread initThread;

    void asyncInit();

public:
CodeManager();
~CodeManager();

    bool init();
    void initSplashScene();
    void run();
    void shutdown();

};

// CodeManager.cpp
#include "CodeManager.h"
#include <chrono>
#include <epoxy/gl.h>
#include <SDL2/SDL.h>

CodeManager::CodeManager() : isInitialized(false), splashSceneRunning(false) {}

CodeManager::~CodeManager() {
if (initThread.joinable()) {
initThread.join();
}
}

void CodeManager::asyncInit() {
// Initialisierung der Manager
eventManager = std::make_unique<EventManager>();
configFileManager = std::make_unique<ConfigFileManager>("");
settingsManager = std::make_unique<SettingsManager>(*configFileManager);
themeManager = std::make_unique<ThemeManager>(*configFileManager);
displayManager = std::make_unique<DisplayManager>(eventManager.get());
soundManager = std::make_unique<SoundManager>();
textManager = std::make_unique<TextManager>(eventManager.get());

    // Weitere Initialisierung
    const auto &settings = settingsManager->getSettings();
    applyTheme(settings.gfxTheme);
    initEventListeners();

    // Initialisierung abgeschlossen
    isInitialized = true;
    splashSceneRunning = false;

}

void CodeManager::initSplashScene() {
splashSceneRunning = true;

    // SplashScreen erstellen
    TextureManager textureManager;
    texture tex;
    textureManager.load("../images/current-logo.png", tex);
    splashScreen = std::make_unique<SplashScreen>(&tex.textureProperties);

    SoundManager soundManager;
    soundManager.loadMusic("../tests/themes/test/music/Pixel Breaker.mp3");
    soundManager.playMusic();

    // Initialisierung im Hintergrund starten
    initThread = std::thread(&CodeManager::asyncInit, this);

    // SplashScene anzeigen
    SDL_Event event;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (splashSceneRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                splashSceneRunning = false;
            }
        }
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        splashScreen->draw();
        SDL_GL_SwapWindow(displayManager->sdlWindow);

        // Beenden der SplashScene, wenn Initialisierung abgeschlossen
        if (isInitialized) {
            splashSceneRunning = false;
        }
    }

}

bool CodeManager::init() {
initSplashScene();
return isInitialized;
}

void CodeManager::run() {
// Hauptprogramm-Logik
}
```
