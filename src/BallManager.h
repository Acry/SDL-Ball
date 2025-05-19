#pragma once

#include <vector>
#include <memory>

#include "Ball.h"
#include "Paddle.h"
#include "EventManager.h"
#include "EffectManager.h"
#include "Brick.h"

// Maximale Anzahl von Bällen im Spiel
constexpr int MAX_BALLS = 8;

class BallManager {
private:
    std::vector<std::unique_ptr<Ball>> balls;
    EventManager* eventManager;

    // Texturen für normale, Feuer- und Tracer-Effekte
    SpriteSheetAnimation normalBallTexture;
    SpriteSheetAnimation fireBallTexture;
    SpriteSheetAnimation tracerTexture;

    // Durchschnittliche Ballgeschwindigkeit berechnen
    float calculateAverageBallSpeed() const;

public:
    // Konstruktor mit EventManager und TextureManager
    explicit BallManager(EventManager* eventMgr);

    // Texturen laden
    bool loadTextures(const TextureManager& texManager);

    // Ball-Management-Funktionen
    void createInitialBall();
    void clearAllBalls();
    void multiplyActiveBalls();
    void releaseGluedBalls();

    // Spawn neuer Bälle
    void spawnBall(float posX, float posY, bool glued = false, float gluedX = 0.0f,
                  float speed = 0.3f, float angle = 0.0f);

    // Update und Render
    void update(float deltaTime, const Paddle& paddle);
    void draw(float deltaTime);

    // Powerup-Anwendung auf alle aktiven Bälle
    void applyPowerup(int powerupType);

    // Kollisionsüberprüfung
    int checkPaddleCollisions(const Paddle& paddle, EffectManager& effectManager);
    void checkBrickCollisions(Brick& brick, EffectManager& effectManager);

    // Getter
    size_t getActiveBallCount() const;
    float getAverageBallSpeed() const;

    // Speziell für Tests und Debugging
    Ball* getBallAt(size_t index);
};
