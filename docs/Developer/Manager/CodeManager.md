# CodeManager

The code manager holds the timing and flow of the program.

Der CodeManager instanziiert alle anderen Manager und
steuert den Ablauf. Er spricht direkt mit dem SettingsManager, dem Theme-Manager und dem configfileManager.
Der [EventManager](EventManager.md) ist die zentrale Kommunikationsschnittstelle.

```text
SettingsManager → ThemeManager → TextureManager → GameManager
       ↑               ↑             ↑
       └───────────────┴─────────────┘
              ConfigFileManager
                 CodeManager
```

In `main` liefe dann der CodeManager und würde die andere Manager initialisieren.
Der GameManager startet die GameLoop, wenn der CodeManager das Event `GameStart` auslöst.

int main() {
CodeManager codeManager;
codeManager.initialize();
codeManager.run();
codeManager.shutdown();
return EXIT_SUCCESS;
}

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