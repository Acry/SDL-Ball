// colors.h
#pragma once

struct Color {
    float r{1.0f};
    float g{1.0f};
    float b{1.0f};
    float a{1.0f};
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
