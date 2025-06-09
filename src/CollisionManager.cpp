// CollisionManager.cpp
#include "CollisionManager.h"

#include "GameEvents.h"

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

CollisionManager::CollisionManager(IEventManager *eventMgr) : eventManager(eventMgr) {
}

// AABB (Axis-Aligned Bounding Box)
bool CollisionManager::checkCollision(const ICollideable &obj1, const ICollideable &obj2) const {
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
    const bool collision = horizontalCollision && verticalOverlap;
    if (collision) {
        GameEvent eventType = {};

        // Bestimme Event-Typ basierend auf Kollisionstypen
        const auto type1 = obj1.getCollisionType();
        const auto type2 = obj2.getCollisionType();

        if (type1 == CollisionType::Ball) {
            switch (type2) {
                case CollisionType::Paddle:
                    eventType = GameEvent::BallHitPaddle;
                    break;
                case CollisionType::Brick:
                    eventType = GameEvent::BallHitBrick;
                    break;
                case CollisionType::BorderLeft:
                    eventType = GameEvent::BallHitLeftBorder;
                    break;
                case CollisionType::BorderRight:
                    eventType = GameEvent::BallHitRightBorder;
                    break;
                case CollisionType::BorderTop:
                    eventType = GameEvent::BallHitTopBorder;
                    break;
                default: return collision;
            }
        } else if (type1 == CollisionType::Paddle) {
            switch (type2) {
                case CollisionType::PowerUp:
                    eventType = GameEvent::PowerUpCollected;
                    break;
                case CollisionType::BorderLeft:
                    eventType = GameEvent::PaddleHitLeftBorder;
                    break;
                case CollisionType::BorderRight:
                    eventType = GameEvent::PaddleHitRightBorder;
                    break;
                default: return collision;
            }
        }

        CollisionData data;
        data.object1 = &obj1;
        data.object2 = &obj2;
        data.hitX = obj1.getPosX();
        data.hitY = obj1.getPosY();
        eventManager->emit(eventType, data);
    }
    return collision;
}

bool CollisionManager::checkCollision(const ICollideable &obj1, const ICollideable &obj2,
                                      float &hitX, float &hitY) const {
    // Prüfe zuerst die einfache AABB-Kollision
    if (!checkCollision(obj1, obj2)) {
        return false;
    }

    // Bei erfolgreicher Kollision setze Kollisionspunkt als Mittelpunkt der Überlappung
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
