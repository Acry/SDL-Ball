#pragma once
#include <epoxy/gl.h>

#include "EventManager.h"
#include "MovingObject.h"
#include "Tracer.h"
#include "GrowableObject.h"

class Ball final: public MovingObject, public GrowableObject {
    GLfloat rad;
    bool eyeCandy;

    EventManager *eventManager;

protected:
    void onSizeChanged() override;

public:
    Tracer tracer;
    bool explosive{false};
    bool glued{false};
    GLfloat gluedX{0.0f};
    GLfloat bsin[32]{}, bcos[32]{};
    bool aimdir{false};
    SpriteSheetAnimation fireTex;

    void launchFromPaddle();

    explicit Ball(EventManager *eventMgr);

    void init() override;

    // Implementation der virtuellen Getter/Setter aus GrowableObject
    [[nodiscard]] GLfloat getWidth() const override { return MovingObject::width; }
    [[nodiscard]] GLfloat getHeight() const override { return MovingObject::height; }
    void setWidth(GLfloat w) override { MovingObject::width = w; }
    void setHeight(GLfloat h) override { MovingObject::height = h; }

    void hit(GLfloat c[]);

    void update(float deltaTime) override;

    void draw(float deltaTime) override;

    GLfloat getRad();

    void setAngle(GLfloat o);

    void setSpeed(GLfloat v, GLfloat maxSpeed = 1.5f);

    void setSize(GLfloat s);

    // im moment nur für den Tracer
    void setEyeCandy(bool value) { eyeCandy = value; }
};
