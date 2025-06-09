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

    // FIXME: stinks, because all GameObjects have Type SpritesheetAnimation
    // which needs update anyway
    void update(const float deltaTime) override;
};
