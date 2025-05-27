#pragma once

#include <SDL2/SDL_stdinc.h>
#include <string>
#include <epoxy/gl.h>

/*
 * Die beiden Farbparameter im SpriteSheet-Konfigurationsfile haben folgende Bedeutung:
 *
 * texcolor=FFFFFFFF - Dies ist die Texturfarbe im RGBA-Format:
 *
 * Definiert eine Farbe, mit der die Textur multipliziert wird
 * Format ist Hexadezimal: RRGGBBAA (Rot, Grün, Blau, Alpha)
 * "FFFFFFFF" bedeutet Weiß mit voller Deckkraft (keine Farbänderung)
 * Wird in readTexProps() in normalisierte Gleitkommawerte (0.0-1.0) für OpenGL konvertiert
 * parcolor=FFFFFF - Dies ist die Partikelfarbe im RGB-Format:
 *
 * Definiert die Farbe für zugehörige Partikeleffekte
 * Format ist Hexadezimal: RRGGBB (Rot, Grün, Blau)
 * "FFFFFF" bedeutet Weiß
 * Der Kommentar "//no effect" deutet darauf hin, dass diese Einstellung in diesem speziellen Fall keine Auswirkung hat.
*/
struct SpriteSheetAnimationProperties {
    GLuint texture;
    GLfloat xoffset;
    GLfloat yoffset;
    int cols, rows; // rows and columns are in this spritesheet
    int ticks;
    Uint32 frames;
    bool direction; // Does the animation loop from 0 -> X -> 0 or from 0 -> X - 0
    bool playing; // Is the animation currently playing?

    bool padding;
    // Bit of a nasty hack, but if a texture is padded with 1 pixel around each frame, this have to be set to 1
    float pxw, pxh; // pixel width, and height

    GLfloat glTexColorInfo[4];
    GLfloat glParColorInfo[3]; // This and above replaced object::color and particle colors

    std::string fileName; // Quite the fugly.. This will be set by readTexProps();
};

enum class TextureProperty {
    XOffset,
    YOffset,
    Cols,
    Rows,
    Ticks,
    Frames,
    Bidir,
    Playing,
    Padding,
    TexColor,
    ParColor,
    FileName,
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
    Uint32 frame; // Current animation frame
    bool playing; // Animation playing state
    bool firstFrame; // First frame flag
    texPos texturePosition; // Current frame texture coordinates
    SpriteSheetAnimationProperties textureProperties; // Texture properties
    SpriteSheetAnimation();
};
