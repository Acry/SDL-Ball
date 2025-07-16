// GameManager.cpp
#include "GameManager.h"
#include <SDL2/SDL_log.h>

GameManager::GameManager()
    : gameRunning(false), gamePaused(false), currentLevel(1), score(0) {
    init();
}

GameManager::~GameManager() {
    // delete ballManager;
    delete brickManager;
    // delete powerupManager;
    delete backgroundManager;
    delete effectManager;
    // delete player; // paddle
}

void GameManager::init() {
    SDL_Log("GameManager: Initialisierung...");

    initializeManagers();

    loadResources();

    initializeGameObjects();

    // Spielgrenzen einrichten
    setupBorders();

    setupEventHandlers();

    SDL_Log("GameManager: Initialisierung abgeschlossen");
}

void GameManager::initializeManagers() {
    // Grundlegende Manager initialisieren

    configManager = new ConfigFileManager("./test");
    settingsManager = new SettingsManager(configManager);
    displayManager = new DisplayManager(settingsManager.getVideoSettings());
    themeManager = new ThemeManager(configManager);
    soundManager = new SoundManager();
    soundManager.setSoundTheme(themeManager.getCurrentSoundTheme());
    textureManager = new TextureManager();
    textureManager.setSpriteTheme(themeManager.getCurrentSpriteTheme());
    backgroundManager = new BackgroundManager(textureManager, themeManager);
    backgroundManager.updateBgIfNeeded(currentLevel, themeManager.getCurrentBackgroundTheme());
    textManager = new TextManager();
    textManager.setTheme(themeManager.getCurrentFontTheme());
    effectManager = new EffectManager(eventManager, textureManager);
    effectManager->setTheme(themeManager.getCurrentEffectTheme());
    levelManager = new LevelManager(configManager, themeManager);
    saveGameManager = new SaveGameManager(configManager, settingsManager);
    eventManager = new EventManager();
    collisionManager = new CollisionManager();
    highscoreManager = new HighscoreManager(configManager);

    // Spielspezifische initialisieren
    ballManager = new BallManager(textureManager);
    brickManager = new BrickManager(textureManager, themeManager);
    powerupManager = new PowerupManager(textureManager, themeManager);
    player = new Player(textureManager, themeManager);

    // GameObjects initialisieren
    playfieldBorder = new PlayfieldBorder(textureManager);
    ball;
    powerupManager->init();
    effectManager->init();
    player->init();

    // hud initialisieren
    hud.manager;
    menu.manager;
    sceneManager;
}

bool GameManager::initializeGameObjects() {
    // Prüfen, ob alle Texturen geladen wurden
    if (!textureManager.loadAllGameTextures()) {
        SDL_Log("Fehler beim Laden der Spieltexturen");
        return false;
    }

    // Ball-Textur zuweisen
    ball.setTexture(textureManager.getBallTexture(BallTexture::Normal));

    // Paddle-Texturen zuweisen
    paddle.setTexture(textureManager.getPaddleTexture(PaddleTexture::Base));

    // Bei PowerUps/Zustandsänderungen
    paddle.setGlueLayer(false); // Standardmäßig deaktiviert
    paddle.setGunLayer(false); // Standardmäßig deaktiviert

    // Bricks mit verschiedenen Texturen initialisieren
    for (auto &brick: bricks) {
        // Je nach Brick-Typ unterschiedliche Texturen zuweisen
        switch (brick.getType()) {
            case BrickType::Blue:
                brick.setTexture(textureManager.getBrickTexture(BrickTexture::Blue));
                break;
            case BrickType::Green:
                brick.setTexture(textureManager.getBrickTexture(BrickTexture::Green));
                break;
                // usw. für andere Brick-Typen
        }
    }

    return true;
}

void GameManager::setupEventHandlers() {
    // Event-Handler für Kollisionen
    eventManager.addListener(GameEvent::BallHitPaddle,
                             [this](const EventData &data) {
                                 Ball *ball = static_cast<Ball *>(data.sender);
                                 Paddle *paddle = static_cast<Paddle *>(data.target);

                                 // Sound abspielen
                                 soundManager.playSound("paddleHit");

                                 // Effekt erzeugen
                                 effectManager->createPaddleHitEffect(data.posX, data.posY);
                             },
                             this
    );

    eventManager.addListener(GameEvent::BallHitBrick,
                             [this](const EventData &data) {
                                 GameObject *brick = static_cast<GameObject *>(data.target);

                                 // Brick beschädigen
                                 if (brick->damage(1)) {
                                     // Wenn der Brick zerstört wurde
                                     // Score erhöhen
                                     score += data.points;

                                     // Zerstörungsevent auslösen
                                     EventData destroyedData;
                                     destroyedData.posX = data.posX;
                                     destroyedData.posY = data.posY;
                                     destroyedData.points = data.points;
                                     eventManager.emit(GameEvent::BrickDestroyed, destroyedData);

                                     // Powerup-Spawn prüfen
                                     powerupManager->trySpawnPowerup(data.posX, data.posY);

                                     // Level-Abschluss prüfen
                                     if (brickManager->isLevelCompleted()) {
                                         EventData levelData;
                                         eventManager.emit(GameEvent::LevelCompleted, levelData);
                                     }
                                 }

                                 // Sound abspielen
                                 soundManager.playSound("brickHit");

                                 // Effekt erzeugen
                                 effectManager->createBrickHitEffect(data.posX, data.posY);
                             },
                             this
    );

    eventManager.addListener(GameEvent::PowerUpCollected,
                             [this](const EventData &data) {
                                 Powerup *powerup = static_cast<Powerup *>(data.sender);

                                 // Powerup aktivieren
                                 powerupManager->activatePowerup(powerup);

                                 // Sound abspielen
                                 soundManager.playSound("powerup");

                                 // Effekt erzeugen
                                 effectManager->createPowerupEffect(data.posX, data.posY);
                             },
                             this
    );

    eventManager.addListener(GameEvent::BallLost,
                             [this](const EventData &data) {
                                 // Leben abziehen
                                 player->loseLife();

                                 // Sound abspielen
                                 soundManager.playSound("ballLost");

                                 // Game Over prüfen
                                 if (player->getLives() <= 0) {
                                     EventData gameOverData;
                                     eventManager.emit(GameEvent::GameOver, gameOverData);
                                 } else {
                                     // Neuen Ball erzeugen
                                     ballManager->resetBall();
                                 }
                             },
                             this
    );

    eventManager.addListener(GameEvent::GameOver,
                             [this](const EventData &data) {
                                 // Spiel beenden
                                 gameOver();
                             },
                             this
    );

    eventManager.addListener(GameEvent::LevelCompleted,
                             [this](const EventData &data) {
                                 // Nächstes Level
                                 goToNextLevel();
                             },
                             this
    );

    eventManager->addListener(GameEvent::BallCreated,
                              [this](const EventData &data) { effectManager->handleBallCreated(data); }, this);
    eventManager->addListener(GameEvent::BallMoved,
                              [this](const EventData &data) { effectManager->handleBallMoved(data); }, this);
    eventManager->addListener(GameEvent::BallDestroyed,
                              [this](const EventData &data) { effectManager->handleBallDestroyed(data); }, this);
}

void GameManager::update(float deltaTime) {
    if (!gameRunning || gamePaused)
        return;

    // Manager aktualisieren
    ballManager->update(deltaTime);
    powerupManager->update(deltaTime);
    player->update(deltaTime);
    effectManager->update(deltaTime);

    // Kollisionen prüfen
    checkCollisions();
}

void GameManager::checkCollisions() {
    Paddle &paddle = player->getPaddle();

    float hitX, hitY;
    Paddle &paddle = player->getPaddle();

    // Paddle-Border-Kollisionen generisch prüfen
    for (auto *border: borders) {
        // borders wäre ein Array oder Vektor von ICollideable*
        if (CollisionManager::checkCollisionWithBorder(paddle, *border, hitX, hitY)) {
            border->onCollision(&paddle, hitX, hitY);

            // Optional: Korrektur der Paddle-Position
            int borderType = border->getCollisionType();
            if (borderType == static_cast<int>(CollisionType::BorderLeft)) {
                paddle.pos_x = border->getPosX() + border->getWidth() + paddle.getWidth();
            } else if (borderType == static_cast<int>(CollisionType::BorderRight)) {
                paddle.pos_x = border->getPosX() - border->getWidth() - paddle.getWidth();
            }
        }
    }

    // Ball-Border Kollisionen
    for (Ball *ball: ballManager->getBalls()) {
        int side;
        if (CollisionManager::checkBallBorderCollision(*ball, border, side)) {
            // Bei Kollision mit unterem Rand = Ball verloren
            if (side == 0) {
                EventData data;
                data.sender = ball;
                eventManager.emit(GameEvent::BallLost, data);
                ball->setActive(false);
            } else {
                // Normale Border-Kollision
                EventData data;
                data.posX = ball->pos_x;
                data.posY = ball->pos_y;
                data.sender = ball;
                eventManager.emit(GameEvent::BallHitBorder, data);

                // Ball abprallen lassen
                if (side == 1) {
                    // Oben
                    ball->yvel = -ball->yvel;
                } else if (side == 2 || side == 3) {
                    // Links oder rechts
                    ball->xvel = -ball->xvel;
                }
            }
        }
    }

    // Alle anderen Kollisionen
    for (Ball *ball: ballManager->getBalls()) {
        if (ball->isActive()) {
            CollisionManager::checkGameCollisions(
                *ball,
                paddle,
                brickManager->getActiveBricks(),
                powerupManager->getActivePowerups(),
                eventManager
            );
        }
    }
}

void GameManager::draw(float deltaTime) {
    // SceneManager.render?
    // Hintergrund rendern
    backgroundManager->draw();

    // Spielobjekte rendern
    brickManager->draw();
    // powerupManager->draw();
    // player->draw();
    // ballManager->draw();
    // effectManager->draw();

    // UI-Elemente rendern
    // HUD-Manager zeichnen
    // textManager.write("Score: " + std::to_string(score), 10, 10, 24);
    // textManager.write("Lives: " + std::to_string(player->getLives()), 10, 40, 24);
}

void GameManager::handleEvents(SDL_Event &event) {
    // Spieler-Eingaben
    player->handleEvent(event);

    // Spiel-Steuerung
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                pauseGame();
                break;
            case SDLK_SPACE:
                // Ball starten, falls er am Paddle klebt
                ballManager->launchBalls();
                break;
        }
    }
}

void GameManager::applySettings() {
    // Theme aus Settings anwenden
    const settings &s = settingsManager.getSettings();
    themeManager.setCurrentTheme(s.gfxTheme);

    // Sound-Lautstärke anwenden
    soundManager.setVolume(s.soundVolume);
}

void GameManager::startGame() {
    gameRunning = true;
    gamePaused = false;

    // Spiel zurücksetzen
    score = 0;
    currentLevel = 1;

    // Level laden
    brickManager->loadLevel(currentLevel);

    // Spieler zurücksetzen
    player->reset();

    // Ball zurücksetzen
    ballManager->resetBall();
}

void GameManager::pauseGame() {
    gamePaused = true;
}

void GameManager::resumeGame() {
    gamePaused = false;
}

void GameManager::restartLevel() {
    // Aktuelles Level neu laden
    brickManager->loadLevel(currentLevel);

    // Ball zurücksetzen
    ballManager->resetBall();

    // Powerups entfernen
    powerupManager->clearPowerups();
}

void GameManager::goToNextLevel() {
    currentLevel++;

    // Nächstes Level laden oder Spiel beenden
    if (brickManager->loadLevel(currentLevel)) {
        // Ball zurücksetzen
        ballManager->resetBall();

        // Powerups entfernen
        powerupManager->clearPowerups();
    } else {
        // Alle Level geschafft
        gameRunning = false;
    }
}

void GameManager::gameOver() {
    gameRunning = false;

    // Hier könntest du den Highscore speichern

    SDL_Log("Game Over! Finaler Score: %d", score);
}

void GameManager::quitGame() {
    // Ressourcen freigeben;
    delete Managers;
};

void GameManager::activatePowerUp(PowerUpType type) {
    switch (type) {
        case PowerUpType::Glue:
            paddle.setGlueLayer(true);
            break;
        case PowerUpType::Gun:
            paddle.setGunLayer(true);
            break;
        case PowerUpType::Fireball:
            ball.setTexture(textureManager.getBallTexture(BallTexture::Fireball));
            break;
            // weitere PowerUp-Typen
    }
}
