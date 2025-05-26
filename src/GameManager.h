// GameStateManager.h
#pragma once

#include "BackgroundManager.h"
#include "BallManager.h"
#include "ConfigFileManager.h"
#include "Display.hpp"
#include "Paddle.h"
#include "Player.h"
#include "SaveGameManager.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "ThemeManager.h"
#include "TextManager.h"
#include "BrickManager.h"
#include "PowerupManager.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "PlayfieldBorder.h"
#include "EffectManager.h"

class GameManager {
    // Grundlegende Manager
    ConfigFileManager& configManager;
    SettingsManager& settingsManager;
    ThemeManager& themeManager;
    TextureManager textureManager;
    SoundManager soundManager;
    TextManager textManager;
    EventManager eventManager;

    // Spielspezifische Manager
    BallManager* ballManager;
    BrickManager* brickManager;
    PowerupManager* powerupManager;
    BackgroundManager* backgroundManager;
    EffectManager* effectManager;
    Player* player;

    PlayfieldBorder border;

    // Spielzustand
    bool gameRunning;
    bool gamePaused;
    int currentLevel;
    int score;
    int currentScene;
    currentDifficulty;
    // Initialisierungsmethoden
    void initializeManagers();
    void setupEventHandlers();

    // Kollisionsprüfung
    void checkCollisions();

public:
    GameManager();
    ~GameManager();

    // Hauptmethoden für den Spielablauf
    void init();
    void update(float deltaTime);
    void render(float deltaTime);
    void handleEvents(SDL_Event& event);

    // Spielsteuerungsmethoden
    void startGame();
    void pauseGame();
    void resumeGame();
    void restartLevel();
    void goToNextLevel();
    void gameOver();
    void quitGame();
    // Settings-Anwendung
    void applySettings();
};