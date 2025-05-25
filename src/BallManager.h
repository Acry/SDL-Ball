// BallManager.h
#pragma once

#include "Ball.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Paddle.h"
#include "ICollideable.h"
#include <vector>
#include <memory>

class BallManager {
private:
    std::vector<std::unique_ptr<Ball>> balls;
    SpriteSheetAnimation normalBallTexture;
    SpriteSheetAnimation fireBallTexture;
    SpriteSheetAnimation tracerTexture;
    EventManager* eventManager;

    static constexpr size_t MAX_BALLS = 16;

    float calculateAverageBallSpeed() const;

public:
    explicit BallManager(EventManager* eventMgr);
    ~BallManager();

    bool loadTextures(const TextureManager& texManager);

    // Ball-Erstellung und -Verwaltung
    void createInitialBall();
    void spawnBall(float posX, float posY, bool glued = false,
                   float gluedX = 0.0f, float speed = 0.3f, float angle = 0.0f);
    void clearAllBalls();

    // Ball-Aktionen
    void multiplyActiveBalls();
    void releaseGluedBalls();
    void applyPowerup(int powerupType);

    // Update & Draw
    void update(float deltaTime, const Paddle& paddle);
    void draw(float deltaTime);

    // Hilfsmethoden
    size_t getActiveBallCount() const;
    float getAverageBallSpeed() const;

    // Zugriff auf Bälle
    Ball* getBallAt(size_t index);
    std::vector<Ball*> getBalls();

    // Kollisions-Integration
    std::vector<ICollideable*> getCollideables() const {
        std::vector<ICollideable*> collideables;
        collideables.reserve(balls.size());
        for (const auto& ball : balls) {
            collideables.push_back(const_cast<Ball*>(ball.get()));
        }
        return collideables;
    }
};