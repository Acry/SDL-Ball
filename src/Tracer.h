#pragma once
#include <epoxy/gl.h>
#include "SpriteSheetAnimation.h"

class Tracer {
    GLfloat x[100], y[100];      // Position
    GLfloat r[100], g[100], b[100]; // Farbe
    GLfloat a[100];              // Alpha wird stärker reduziert je weiter vom Ball
    GLfloat s[100];              // Größe wird kleiner je weiter vom Ball
    GLfloat cr, cg, cb;          // Aktuelle Farben
    bool active[100];
    int color;
    GLfloat lastX, lastY;        // Letzte Position

public:
    GLfloat height, width;
    SpriteSheetAnimation tex;
    int len;

    Tracer();
    void draw(float deltaTime);
    void colorRotate(bool explosive, const GLfloat c[]);
    void update(GLfloat nx, GLfloat ny);
};