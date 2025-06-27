// colors.h
#pragma once

#include <epoxy/gl.h>

struct Color {
    GLfloat r{0.0f};
    GLfloat g{0.0f};
    GLfloat b{0.0f};
    GLfloat a{0.0f};

    GLfloat &operator[](const int i) {
        return *(&r + i);
    }

    const GLfloat &operator[](const int i) const {
        return *(&r + i);
    }
};

#define GL_WHITE 1, 1, 1, 1
#define GL_BLACK 0, 0, 0, 1
#define GL_FULL_RED 1, 0, 0, 1
#define GL_FULL_GREEN 0, 1, 0, 1
#define GL_FULL_BLUE 0, 0, 1, 1
#define GL_DARK_BLUE 0.0, 0.0, 0.4, 1.0
#define GL_ORANGE 1.0, 0.647, 0.0, 1.0
#define GL_LIGHT_BLUE 0.0627, 0.4784, 0.6902, 1.0
#define GL_DEEP_ORANGE 0.8, 0.333, 0.0, 1.0
#define GL_BRIGHT_ORANGE 1.0, 0.7, 0.0, 1.0

// Tiefes, sattes Dunkelgrau
constexpr Color darkGray = {0.25f, 0.25f, 0.25f, 1.0f};

// Mittleres sattes Grau (Anthrazit)
constexpr Color mediumGray = {0.33f, 0.33f, 0.33f, 1.0f};

// Sattes, volles Grau
constexpr Color richGray = {0.4f, 0.4f, 0.4f, 1.0f};

// Elegantes Grau mit leichtem Blaustich
constexpr Color blueishGray = {0.35f, 0.35f, 0.4f, 1.0f};

// Warmes Grau mit leichtem Rotstich
constexpr Color warmGray = {0.4f, 0.38f, 0.36f, 1.0f};
