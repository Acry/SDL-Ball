// GameObject.h
#pragma once

#include <epoxy/gl.h>
#include "SpriteSheetAnimation.h"

class EventManager;

class GameObject {
protected:
    EventManager *eventManager{nullptr}; // Zugriff für abgeleitete Klassen

public:
    explicit GameObject(EventManager *eventMgr = nullptr) : eventManager(eventMgr) {
    }

    // Basisattribute für alle Spielobjekte
    GLfloat opacity{1.0f};
    GLfloat pos_x{0.0f}, pos_y{0.0f};
    GLfloat width{0.0f}, height{0.0f};
    bool active{true};
    bool collide{true}; // Think this is not used anymore -> ICollideable interface
    bool reflect{false};
    SpriteSheetAnimation texture;

    // Virtuelle Methoden - Interface-Definition
    virtual ~GameObject() = default;

    virtual void init() = 0;

    virtual void update(float deltaTime) = 0;

    virtual void draw(float deltaTime = 0.0f) = 0;

    // Hilfsfunktionen
    [[nodiscard]] virtual bool isActive() const { return active; }
    [[nodiscard]] bool isVisible() const { return opacity > 0.0f; }
};
