#include "SpriteSheetAnimation.h"


SpriteSheetAnimation::SpriteSheetAnimation() : age(10000),
                                               direction(false),
                                               lastFrame(1000),
                                               frame(1),
                                               firstFrame(true) {
}

void SpriteSheetAnimation::play(const float deltaTime) {
    if (textureProperties.playing) {
        // Verwende den übergebenen deltaTime-Parameter statt der globalen Variable
        age += deltaTime * 1000.0f; // Umwandlung in Millisekunden
        if (age >= textureProperties.ticks) {
            age = 0.0f;

            // Rest der Methode bleibt unverändert
            if (!direction) {
                if (frame == textureProperties.frames) {
                    if (textureProperties.bidir) {
                        direction = true;
                    } else {
                        frame = 1;
                    }
                } else {
                    frame++;
                }
            }

            if (direction) {
                if (frame == 1) {
                    direction = false;
                    frame = 2;
                } else {
                    frame--;
                }
            }
        }
    }

    // Update texture coordinates if frame changed
    if (frame != lastFrame || firstFrame) {
        int row = 0;
        Uint32 f = 0;
        int col = 0;
        lastFrame = frame;
        firstFrame = false;

        // Calculate texture coordinates for current frame
        for (row = 0; row < textureProperties.rows; row++) {
            for (col = 0; col < textureProperties.cols; col++) {
                f++;
                if (f == frame) {
                    // Top left
                    texturePosition[0] = (textureProperties.xoffset * static_cast<float>(col));
                    texturePosition[1] = (textureProperties.yoffset * static_cast<float>(row));

                    // Top right
                    texturePosition[2] = (textureProperties.xoffset * static_cast<float>(col)) + textureProperties.
                                         xoffset;
                    texturePosition[3] = (textureProperties.yoffset * static_cast<float>(row));

                    // Bottom right
                    texturePosition[4] = (textureProperties.xoffset * static_cast<float>(col)) + textureProperties.
                                         xoffset;
                    texturePosition[5] = (textureProperties.yoffset * static_cast<float>(row)) + textureProperties.
                                         yoffset;

                    // Bottom left
                    texturePosition[6] = (textureProperties.xoffset * static_cast<float>(col));
                    texturePosition[7] = (textureProperties.yoffset * static_cast<float>(row)) + textureProperties.
                                         yoffset;

                    // Apply padding if enabled
                    if (textureProperties.padding) {
                        texturePosition[0] += 1.0f / textureProperties.pxw;
                        texturePosition[1] += 1.0f / textureProperties.pxh;

                        texturePosition[2] -= 1.0f / textureProperties.pxw;
                        texturePosition[3] += 1.0f / textureProperties.pxh;

                        texturePosition[4] -= 1.0f / textureProperties.pxw;
                        texturePosition[5] -= 1.0f / textureProperties.pxh;

                        texturePosition[6] += 1.0f / textureProperties.pxw;
                        texturePosition[7] -= 1.0f / textureProperties.pxh;
                    }
                }
            }
        }
    }
}
