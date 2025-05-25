// GameStateManager.h
#pragma once
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
private:
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

    // Spielfeld-Grenzen
    PlayfieldBorder border;

    // Spielzustand
    bool gameRunning;
    bool gamePaused;
    int currentLevel;
    int score;

    // Initialisierungsmethoden
    void initializeManagers();
    void setupEventHandlers();
    void setupBorders();
    void loadResources();

    // Kollisionsprüfung
    void checkCollisions();

public:
    GameManager(ConfigFileManager& cfg, SettingsManager& settings, ThemeManager& theme);
    ~GameManager();

    // Hauptmethoden für den Spielablauf
    void init();
    void update(float deltaTime);
    void render();
    void handleEvent(SDL_Event& event);

    // Spielsteuerungsmethoden
    void startGame();
    void pauseGame();
    void resumeGame();
    void restartLevel();
    void goToNextLevel();
    void gameOver();

    // Settings-Anwendung
    void applySettings();
};