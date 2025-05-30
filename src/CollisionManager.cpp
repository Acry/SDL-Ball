// CollisionManager.cpp
#include <cmath>
#include "CollisionManager.h"

//        +1
//         ^
//         |
// -1 <----+----> +1
//         |
//        -1
// In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
// (0, 1) ------- (1, 1)
//   |              |
//   |              |
//   |              |
// (0, 0) ------- (1, 0)


bool CollisionManager::checkCollision(const ICollideable &obj1, const ICollideable &obj2) {
    //                               LB,               PAD
    const bool horizontalCollision = obj1.getPosX() + obj1.getWidth() > obj2.getPosX() &&
                                     //    RB,               PAD
                                     obj1.getPosX() < obj2.getPosX() + obj2.getWidth();
    // return horizontalCollision;

    //                              B                                   TK
    const bool verticalOverlap = obj1.getPosY() + obj1.getHeight() > obj2.getPosY() &&
                                 //     B                     PAD
                                 obj1.getPosY() < obj2.getPosY() + obj2.getHeight();
    // return verticalOverlap;

    return horizontalCollision && verticalOverlap;
}

bool CollisionManager::checkCollision(const ICollideable &obj1, const ICollideable &obj2,
                                      float &hitX, float &hitY) {
    // Prüfe zuerst die einfache AABB-Kollision
    if (!checkCollision(obj1, obj2)) {
        return false;
    }

    // Bei erfolgreicher Kollision, setze Kollisionspunkt als Mittelpunkt der Überlappung
    float overlapLeft = obj1.getPosX() + obj1.getWidth() - (obj2.getPosX() - obj2.getWidth());
    float overlapRight = obj2.getPosX() + obj2.getWidth() - (obj1.getPosX() - obj1.getWidth());
    float overlapTop = obj1.getPosY() + obj1.getHeight() - (obj2.getPosY() - obj2.getHeight());
    float overlapBottom = obj2.getPosY() + obj2.getHeight() - (obj1.getPosY() - obj1.getHeight());

    // Finde die kleinste Überlappung (die wahrscheinlichste Kollisionsseite)
    float minOverlapX = std::min(overlapLeft, overlapRight);
    float minOverlapY = std::min(overlapTop, overlapBottom);

    if (minOverlapX < minOverlapY) {
        // X-Achsen-Kollision
        hitY = obj1.getPosY();
        if (obj1.getPosX() < obj2.getPosX()) {
            hitX = obj1.getPosX() + obj1.getWidth();
        } else {
            hitX = obj1.getPosX() - obj1.getWidth();
        }
    } else {
        // Y-Achsen-Kollision
        hitX = obj1.getPosX();
        if (obj1.getPosY() < obj2.getPosY()) {
            hitY = obj1.getPosY() + obj1.getHeight();
        } else {
            hitY = obj1.getPosY() - obj1.getHeight();
        }
    }

    return true;
}

void CollisionManager::processCollisions(const std::vector<ICollideable *> &objects) {
    // Alle Objekte miteinander auf Kollisionen prüfen
    const size_t objectCount = objects.size();

    for (size_t i = 0; i < objectCount; ++i) {
        auto *obj1 = objects[i];
        if (!obj1->isActive()) continue;

        for (size_t j = i + 1; j < objectCount; ++j) {
            auto *obj2 = objects[j];
            if (!obj2->isActive()) continue;

            float hitY = 0.0f;
            if (float hitX = 0.0f; checkCollision(*obj1, *obj2, hitX, hitY)) {
                // Kollision erkannt, benachrichtige beide Objekte
                obj1->onCollision(obj2, hitX, hitY);
                obj2->onCollision(obj1, hitX, hitY);
            }
        }
    }
}
