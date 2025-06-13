// SpriteSheetAnimation.h
#pragma once

#include <SDL2/SDL_stdinc.h>

struct SpriteSheetAnimationProperties {
    float xoffset; // Größe eines Frames im UV-Raum
    float yoffset; // Größe eines Frames im UV-Raum
    int cols; // Anzahl der Spalten im Spritesheet
    int rows; // Anzahl der Zeilen im Spritesheet
    int ticks; // Zeit pro Frame (ms)
    Uint32 frames; // Anzahl der Frames
    bool bidir; // Bidirektionale Animation
    bool playOnce;
    bool playing; // Initial abspielen?
    bool padding; // Anti-Bleeding Padding
    float pxw;
    float pxh; // Pixel-Dimensionen für GL-Operationen
};

enum class AnimationProperty {
    XOffset,
    YOffset,
    Cols,
    Rows,
    Ticks,
    Frames,
    Bidir,
    PlayOnce,
    Playing,
    Padding,
    PixelWidth,
    PixelHeight,
    Unknown
};

// TODO: Use this in the whole project
struct TextureCoordinates {
    // In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
    // (0, 1) ------- (1, 1)
    //   |              |
    //   |              |
    //   |              |
    // (0, 0) ------- (1, 0)
    float coords[8]{
        0.0f, 0.0f, // Bottom-left corner (u, v)
        1.0f, 0.0f, // Bottom-right corner (u, v)
        1.0f, 1.0f, // Top-right corner (u, v)
        0.0f, 1.0f // Top-left corner (u, v)
    };

    // Zugriff auf einzelne Koordinaten
    float &operator[](const size_t index) { return coords[index]; }
    const float &operator[](const size_t index) const { return coords[index]; }
};

class SpriteSheetAnimation {
    float currentFrameAge;
    bool currentDirection;

public:
    void play(float deltaTime = 0.0f);

    void calculateTextureCoordinates();

    void reset();

    Uint32 currentFrame;
    bool isPlaying;
    TextureCoordinates currentTexturePosition{};
    SpriteSheetAnimationProperties animationProperties;

    explicit SpriteSheetAnimation(const SpriteSheetAnimationProperties &props);
};
