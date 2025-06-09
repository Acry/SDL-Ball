// CollisionManager.h
#pragma once

#include "ICollideable.h"
#include "IEventManager.h"

struct CollisionPoint {
    float x;
    float y;
};

class CollisionManager {
public:
    explicit CollisionManager(IEventManager *eventMgr);

    // Einfache Kollisionserkennung
    [[nodiscard]] bool checkCollision(const ICollideable &obj1, const ICollideable &obj2) const;

    // Spezialisierte Kollisionserkennung mit Kollisionspunkt
    bool checkCollision(const ICollideable &obj1, const ICollideable &obj2,
                               float &hitX, float &hitY) const;

private:
    IEventManager *eventManager;
};
