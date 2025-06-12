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
    Playing,
    Padding,
    PixelWidth,
    PixelHeight,
    Unknown
};

// Texture coordinates array type (8 floats for 4 vertices)
using texPos = float[8];

class SpriteSheetAnimation {
    float age; // Current frame age
    bool direction; // Animation direction for bidirectional
    Uint32 lastFrame; // Last rendered frame

public:
    void play(float deltaTime = 0.0f); // Animationslogik
    void calculateTextureCoordinates();

    Uint32 frame; // Current animation frame
    bool playing; // Animation playing state
    bool firstFrame; // First frame flag
    texPos texturePosition{}; // Current frame texture coordinates
    SpriteSheetAnimationProperties animationProperties; // Texture properties
    explicit SpriteSheetAnimation(const SpriteSheetAnimationProperties &props);
};
