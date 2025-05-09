#pragma once

using uint = unsigned int;
#include "texture_properties.h"

// Texture coordinates array type (8 floats for 4 vertices)
using texPos = float[8];

class texture {
    float age;         // Current frame age
    bool dir;          // Animation direction for bidirectional
    uint lastframe;    // Last rendered frame

public:
    uint frame;        // Current animation frame
    bool playing;      // Animation playing state
    bool firstFrame;   // First frame flag
    texPos pos;        // Current frame texture coordinates
    texProp prop;      // Texture properties

    texture();
    void play();
};
