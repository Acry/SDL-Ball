// MathHelper.h
#pragma once
#include <epoxy/gl.h>

#define RAD 6.28318530718f // 2*PI
constexpr float PI_F = 3.14159265358979323846f;

struct position {
    GLfloat x;
    GLfloat y;
};

float randomFloat(const float total, const float negative);

int randomInt(int min, int max);
