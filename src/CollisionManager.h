// CollisionManager.h
#pragma once

#include "GameObject.h"
#include "Ball.h"
#include "Paddle.h"

struct CollisionPoint {
    float x;
    float y;
};

class CollisionManager {
public:
    // Allgemeine Kollisionserkennung zwischen zwei GameObjects
    static bool checkCollision(const GameObject& obj1, const GameObject& obj2);

    // Spezialisierte Kollisionserkennung mit Details
    static bool checkBallPaddleCollision(Ball& ball, const Paddle& paddle, CollisionPoint& collisionPoint);

    // Weitere spezialisierte Kollisionsmethoden...
    static bool checkBallBrickCollision(Ball& ball, GameObject& brick, CollisionPoint& collisionPoint);

private:
    // Hilfsfunktionen für Kollisionsberechnungen
    static float calculateBounceAngle(float paddleWidth, float paddleX, float hitX);
};
