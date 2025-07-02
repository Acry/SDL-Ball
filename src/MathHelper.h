// MathHelper.h
#pragma once
#include <epoxy/gl.h>

#define BOUNCE_ANGLE_RANGE 2.61799388 // // 150 Grad in Radiant
#define MIN_BOUNCE_ANGLE 0.261799388 // 15 Grad in Radiant

constexpr float RAD_F = 6.28318530718f; // 2*PI
constexpr float PI_F = 3.14159265358979323846f;

struct position {
    GLfloat x;
    GLfloat y;
};

float randomFloat(float total, float negative);

int randomInt(int min, int max);
