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
public:
    // Einfache Kollisionserkennung
    static bool checkCollision(const ICollideable &obj1, const ICollideable &obj2);

    // Spezialisierte Kollisionserkennung mit Kollisionspunkt
    static bool checkCollision(const ICollideable &obj1, const ICollideable &obj2,
                               float &hitX, float &hitY);

    // Prüfe alle Kollisionen zwischen Objekten und leite Events weiter
    static void processCollisions(const std::vector<ICollideable *> &objects);

    static void handleBallBricksCollisions(Ball& ball, std::vector<Brick>& bricks);

    static void handleBallBorderCollisions(Ball& ball,
                                      const PlayfieldBorder& leftBorder,
                                      const PlayfieldBorder& rightBorder,
                                      const PlayfieldBorder& topBorder);

    static void handleBallPaddleCollision(Ball& ball, const Paddle& paddle);

    static void handlePaddleBorderCollisions(Paddle& paddle,
                                            const PlayfieldBorder& leftBorder,
                                            const PlayfieldBorder& rightBorder);

};
