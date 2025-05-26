// TextureProperties.h
#pragma once

#include <string>
#include <epoxy/gl.h>

// Texture properties struct
struct texProp {
    GLuint texture; // Den GLtexture der er loaded
    GLfloat xoffset; // Hvor stort er springet mellem hver subframe
    GLfloat yoffset; //
    int cols, rows; // hvor mange rækker og kolonner er der i denne textur
    int ticks;
    Uint32 frames; // This many frames in each se
    bool bidir; // Går Looper den fra 0 -> X - 0 eller fra 0 -> X -> 0
    bool playing;

    bool padding;
    // Bit of a nasty hack, but if a texture is padded with 1 pixel around each frame, this have to be set to 1
    float pxw, pxh; // pixel width, and height

    GLfloat glTexColorInfo[4];
    GLfloat glParColorInfo[3]; // This and above replaced object::color and particle colors

    std::string fileName; // Quite the fugly.. This will be set by readTexProps();
};
