#pragma once
#include <SDL2/SDL_stdinc.h>

#include "texture_properties.h"

// Texture coordinates array type (8 floats for 4 vertices)
using texPos = float[8];

class SpriteSheetAnimation {
    float age;         // Current frame age
    bool dir;          // Animation direction for bidirectional
    Uint32 lastFrame;    // Last rendered frame

public:
    Uint32 frame;        // Current animation frame
    bool playing;      // Animation playing state
    bool firstFrame;   // First frame flag
    texPos texturePosition;        // Current frame texture coordinates
    texProp textureProperties;      // Texture properties
    SpriteSheetAnimation();
    void play();
};
