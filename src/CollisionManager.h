// CollisionManager.h
#pragma once

#include "ICollideable.h"

class Paddle;
class PlayfieldBorder;
class Ball;
class Brick;

struct CollisionPoint {
    float x;
    float y;
};

class CollisionManager {
    EventManager *eventManager = nullptr;

public:
    explicit CollisionManager(EventManager *eventMgr);

    // Einfache Kollisionserkennung
    static bool checkCollision(const ICollideable &obj1, const ICollideable &obj2);

    // Spezialisierte Kollisionserkennung mit Kollisionspunkt
    static bool checkCollision(const ICollideable &obj1, const ICollideable &obj2,
                               float &hitX, float &hitY);

    // Prüfe alle Kollisionen zwischen Objekten und leite Events weiter
    static void processCollisions(const std::vector<ICollideable *> &objects);

    void handleBallBricksCollisions(Ball &ball, std::vector<Brick> &bricks) const;

    void handleBallBorderCollisions(Ball &ball,
                                           const PlayfieldBorder &leftBorder,
                                           const PlayfieldBorder &rightBorder,
                                           const PlayfieldBorder &topBorder) const;

    void handleBallPaddleCollision(Ball &ball, const Paddle &paddle) const;

    void handlePaddleBorderCollisions(Paddle &paddle,
                                             const PlayfieldBorder &leftBorder,
                                             const PlayfieldBorder &rightBorder) const;
};
