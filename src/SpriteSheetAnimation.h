#pragma once
#include <SDL2/SDL_stdinc.h>
#include "texture_properties.h"

// Texture coordinates array type (8 floats for 4 vertices)
using texPos = float[8];

class SpriteSheetAnimation {
    float age; // Current frame age
    bool direction; // Animation direction for bidirectional
    Uint32 lastFrame; // Last rendered frame

public:
    void play(float deltaTime = 0.0f); // Animationslogik
    Uint32 frame; // Current animation frame
    bool playing; // Animation playing state
    bool firstFrame; // First frame flag
    texPos texturePosition; // Current frame texture coordinates
    texProp textureProperties; // Texture properties
    SpriteSheetAnimation();
};
