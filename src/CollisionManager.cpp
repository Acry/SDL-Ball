// CollisionManager.cpp
#include <cmath>
#include "CollisionManager.h"

bool CollisionManager::checkCollision(const ICollideable& obj1, const ICollideable& obj2) {
    // Einfache AABB-Kollisionserkennung für rechteckige Objekte
    return (obj1.getPosX() - obj1.getWidth() < obj2.getPosX() + obj2.getWidth() &&
            obj1.getPosX() + obj1.getWidth() > obj2.getPosX() - obj2.getWidth() &&
            obj1.getPosY() - obj1.getHeight() < obj2.getPosY() + obj2.getHeight() &&
            obj1.getPosY() + obj1.getHeight() > obj2.getPosY() - obj2.getHeight());
}

bool CollisionManager::checkCollision(const ICollideable& obj1, const ICollideable& obj2,
                                    float& hitX, float& hitY) {
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

void CollisionManager::processCollisions(const std::vector<ICollideable*>& objects) {
    // Alle Objekte miteinander auf Kollisionen prüfen
    const size_t objectCount = objects.size();

    for (size_t i = 0; i < objectCount; ++i) {
        auto* obj1 = objects[i];
        if (!obj1->isActive()) continue;

        for (size_t j = i + 1; j < objectCount; ++j) {
            auto* obj2 = objects[j];
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

bool CollisionManager::checkCollisionWithBorder(
    const ICollideable& movingObject,
    const ICollideable& border,
    float& hitX,
    float& hitY) {

    // Nutze die vorhandene Kollisionsprüfung
    if (!movingObject.isActive() || !border.isActive() ||
        !checkCollision(movingObject, border))
        return false;

    // Kollisionspunkt je nach Randtyp berechnen
    int borderType = border.getCollisionType();
    if (borderType == static_cast<int>(CollisionType::BorderLeft)) {
        hitX = border.getPosX() + border.getWidth();
        hitY = movingObject.getPosY();
    } else if (borderType == static_cast<int>(CollisionType::BorderRight)) {
        hitX = border.getPosX() - border.getWidth();
        hitY = movingObject.getPosY();
    } else if (borderType == static_cast<int>(CollisionType::BorderTop)) {
        hitX = movingObject.getPosX();
        hitY = border.getPosY() - border.getHeight();
    }
    return true;
}
