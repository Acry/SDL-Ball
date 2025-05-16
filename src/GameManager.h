// GameStateManager.h
#pragma once
#include "BackgroundManager.h"
#include "BallManager.h"
#include "ConfigFileManager.h"
#include "Display.hpp"
#include "Player.h"
#include "SaveGameManager.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "ThemeManager.h"
#include "TtfLegacyGl.h"

class GameManager {
    BackgroundManager backgroundManager;
    BallManager ballManager;
    BrickManager brickManager;
    ConfigFileManager configManager;
    ControllerManager controllerManager;
    Display display;
    InputManager input;
    // LevelManager levelManager;
    Player player;
    SaveGameManager saveGameManager;
    SettingsManager& settings;
    SoundManager soundManager;
    ThemeManager themeManager;
    TtfLegacyGl textManager;

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