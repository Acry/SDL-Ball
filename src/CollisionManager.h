// CollisionManager.h
#pragma once

#include "ICollideable.h"

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

    static bool checkCollisionWithBorder(
        const ICollideable &movingObject,
        const ICollideable &border,
        float &hitX,
        float &hitY);
};
