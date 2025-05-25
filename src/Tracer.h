#pragma once
#include <epoxy/gl.h>

#include "MovingObject.h"

class Tracer : public MovingObject {
    GLfloat x[100], y[100]; // Position
    GLfloat r[100], g[100], b[100]; // Farbe
    GLfloat a[100]; // Alpha
    GLfloat s[100]; // Größe
    GLfloat cr{1.0f}, cg{0.0f}, cb{0.0f}; // Aktuelle Farben
    bool active[100];
    int color{0};
    GLfloat lastX{0.0f}, lastY{0.0f}; // Letzte Position
    int len{100};

public:
    explicit Tracer(EventManager *eventMgr = nullptr);

    void init() override;

    void update(float deltaTime) override;

    void draw(float deltaTime) override;

    void setSize(GLfloat w, GLfloat h);

    void colorRotate(bool explosive, const GLfloat c[]);

    void updatePosition(GLfloat nx, GLfloat ny);
};
