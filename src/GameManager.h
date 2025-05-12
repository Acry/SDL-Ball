// GameStateManager.h
#pragma once
#include "ConfigFileManager.h"
#include "Player.h"
#include "SettingsManager.h"

class GameManager {
    ConfigFileManager configManager;
    SettingsManager& settings;
    Display display;
    InputManager input;
    Player player;
    BallManager ballManager;
    BrickManager brickManager;
public:
    GameManager() :
        configManager(),
        settings(SettingsManager::getInstance(configManager)),
        display(settings.getSettings()),
        input(settings),
        player(),
        ballManager(),
        brickManager()
    {
        init();
    }

    void loop() {
        while (!quit) {
            processInput();
            update();
            render();
        }
    }

    void initializePlayer() {
        player.initDifficulty(settingsManager.getDifficulty());
    }

    Player& getPlayer() { return player; }
};

// In main.cpp
GameManager gameState(configFileManager);
auto& player = gameState.getPlayer();

// main.cpp
int main(int argc, char* argv[]) {
    GameManager game;
    game.loop();
    return EXIT_SUCCESS;
}