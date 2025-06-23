# Scenes

AutoplayScene
CreditsScene

GameOverScene
GameScene
HighscoreScene

LevelDoneScene
MenusScenes
PauseScene
PlayScene
SettingsScene

TitleScene

TutorialScenes/InstructionsScene

- GameDescription 1 Controls
- GameDescription 2 Mechanics
- GameDescription 3 PowerUps

## See

[SceneManager](../Manager/SceneManager.md) for more information about the scene system.

## Sketch

Create a couple of example scenes to demonstrate usage (e.g., MainMenuScene and GameplayScene).

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

