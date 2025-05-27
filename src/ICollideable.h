// ICollideable.h
#pragma once

#include <vector>

// Vorwärtsdeklarationen
class EventManager;

enum class CollisionType {
    None = 0,
    Ball = 1,
    Paddle = 2,
    Brick = 3,
    PowerUp = 4,
    BorderLeft = 5,
    BorderRight = 6,
    BorderTop = 7
};

// Interface für Objekte, die an Kollisionen teilnehmen können
class ICollideable {
public:
    virtual ~ICollideable() = default;

    // Position und Dimensionen für Kollisionsprüfung
    virtual float getPosX() const = 0;

    virtual float getPosY() const = 0;

    virtual float getWidth() const = 0;

    virtual float getHeight() const = 0;

    // Aktiv-Status (soll überprüft werden?)
    virtual bool isActive() const = 0;

    // Optional: Kollisionspunkte für komplexere Formen
    virtual const std::vector<float> *getCollisionPoints() const { return nullptr; }

    // Kollisionsereignis-Handler
    virtual void onCollision(ICollideable *other, float hitX, float hitY) = 0;

    // Kollisionstyp-Identifikation für spezifische Reaktionen
    virtual int getCollisionType() const = 0;
};
