// MovingObject.h
#pragma once

#include <epoxy/gl.h>

#include "GameObject.h"

class MovingObject : public GameObject {
public:
    GLfloat xvel{0.0f}, yvel{0.0f}, velocity{0.0f};

    explicit MovingObject(EventManager *eventMgr = nullptr)
        : GameObject(eventMgr) {
    }

    // Bewegungsupdates als virtuelle Methode deklarieren
    void update(const float deltaTime) override;
};
