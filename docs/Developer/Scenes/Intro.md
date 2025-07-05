# Scenes

- AutoplayScene [Autoplay](misc/Autoplay.md)
- CreditsScene

- GameOverScene
- GameScene
- HighscoreScene

- LevelDoneScene
- MenusScenes
- PauseScene
- PlayScene
- SettingsScene

- TitleScene

TutorialScenes/InstructionsScene

- GameDescription 1 Controls
- GameDescription 2 Mechanics
- GameDescription 3 PowerUps

floating Scene -> MainMenu

DemonstrationScene -> Explains the game mechanics and controls,
e.g., how to use the paddle, ball, and power-ups. with slowmotion, icons and text.

## Scene System

Implemenation TBD

A scene system is a way to manage different states or screens in a game or application. Each scene represents a distinct
part of the application, such as the main menu, gameplay, settings, etc. The scene system allows for easy transitions
between these states and manages the lifecycle of each scene.
A scene system typically includes the following components:

- **Scene**: A class or structure that defines the behavior and content of a specific scene. It usually includes methods
  for initialization, updating, rendering, and cleanup.
- **SceneManager**: A manager that handles the active scene, transitions between scenes, and maintains a list of
  available scenes. It provides methods to add, remove, and switch scenes.
- **Transitions**: Effects or animations that occur when switching from one scene to another, such as fading, sliding,
  or other visual effects.
- **Input Handling**: Mechanisms to manage user input specific to each scene, allowing for different controls or
  interactions depending on the active scene.
- **Lifecycle Management**: Methods to manage the lifecycle of scenes, including initialization when a scene is
  activated, updating during each frame, rendering the scene, and cleanup when the scene is no longer needed.
- **Scene Data**: Data structures to hold information about the current state of the scene, such as player progress,
  settings, or other relevant data that needs to be preserved across scene transitions.
- **Event Handling**: A way to handle events that may occur within a scene, such as user actions, game events, or system
  notifications.
- **Scene Stack**: A stack or queue to manage multiple scenes, allowing for complex navigation patterns like going back
  to a previous scene or pushing new scenes onto the stack.
- **Scene Configuration**: Options to configure each scene, such as background music, visual effects, and other settings
  that can be customized per scene.
- **Debugging Tools**: Utilities to help debug scenes, such as logging scene transitions, tracking performance, and
  visualizing scene states.

## See

[SceneManager](../Manager/SceneManager.md) for more information about the scene system.
[Transitions](screen-transitions.md)

## Sketch

Create a couple of example scenes to demonstrate usage (e.g., MainMenuScene and GameplayScene).

```c++
#include <iostream>
#include "Scene.h"

class MainMenuScene : public Scene {
public:
void initialize() override {
std::cout << "Main Menu Scene Initialized\n";
}

    void update(float deltaTime) override {
        std::cout << "Main Menu Scene Updating (Delta: " << deltaTime << ")\n";
    }

    void render() override {
        std::cout << "Rendering Main Menu Scene\n";
    }

    void cleanup() override {
        std::cout << "Main Menu Scene Cleaned Up\n";
    }

};
```

```c++
class GameplayScene : public Scene {
public:
void initialize() override {
std::cout << "Gameplay Scene Initialized\n";
}

    void update(float deltaTime) override {
        std::cout << "Gameplay Scene Updating (Delta: " << deltaTime << ")\n";
    }

    void render() override {
        std::cout << "Rendering Gameplay Scene\n";
    }

    void cleanup() override {
        std::cout << "Gameplay Scene Cleaned Up\n";
    }

};
```

```c++
#include "SceneManager.h"
#include "Scene.h"
#include <memory>

int main() {
SceneManager sceneManager;

    // Add scenes
    sceneManager.addScene("MainMenu", std::make_unique<MainMenuScene>());
    sceneManager.addScene("Gameplay", std::make_unique<GameplayScene>());

    // Set and run main menu
    sceneManager.setActiveScene("MainMenu");
    sceneManager.update(0.016f); // Simulate 60 FPS
    sceneManager.render();

    // Switch to gameplay
    sceneManager.setActiveScene("Gameplay");
    sceneManager.update(0.016f);
    sceneManager.render();

    // Print active scene name
    std::cout << "Active Scene: " << sceneManager.getActiveSceneName() << "\n";

    return 0;

}
```
