#include "texture.h"

extern int globalTicksSinceLastDraw;

texture::texture() :
    age(10000),
    dir(false),
    lastframe(1000),
    frame(1),
    firstFrame(true)
{
}

void texture::play() {
    if (prop.playing) {
        // Check if frame needs updating
        age += globalTicksSinceLastDraw;
        if (age >= prop.ticks) {
            age = 0.0f;

            // Update frame counter based on direction
            if (!dir) {
                if (frame == prop.frames) {
                    if (prop.bidir) {
                        dir = true;
                    } else {
                        frame = 1;
                    }
                } else {
                    frame++;
                }
            }

            if (dir) {
                if (frame == 1) {
                    dir = false;
                    frame = 2;
                } else {
                    frame--;
                }
            }
        }
    }

    // Update texture coordinates if frame changed
    if (frame != lastframe || firstFrame) {
        int row = 0;
        uint f = 0;
        int col = 0;
        lastframe = frame;
        firstFrame = false;

        // Calculate texture coordinates for current frame
        for (row = 0; row < prop.rows; row++) {
            for (col = 0; col < prop.cols; col++) {
                f++;
                if (f == frame) {
                    // Top left
                    pos[0] = (prop.xoffset * static_cast<float>(col));
                    pos[1] = (prop.yoffset * static_cast<float>(row));

                    // Top right
                    pos[2] = (prop.xoffset * static_cast<float>(col)) + prop.xoffset;
                    pos[3] = (prop.yoffset * static_cast<float>(row));

                    // Bottom right
                    pos[4] = (prop.xoffset * static_cast<float>(col)) + prop.xoffset;
                    pos[5] = (prop.yoffset * static_cast<float>(row)) + prop.yoffset;

                    // Bottom left
                    pos[6] = (prop.xoffset * static_cast<float>(col));
                    pos[7] = (prop.yoffset * static_cast<float>(row)) + prop.yoffset;

                    // Apply padding if enabled
                    if (prop.padding) {
                        pos[0] += 1.0f / prop.pxw;
                        pos[1] += 1.0f / prop.pxh;

                        pos[2] -= 1.0f / prop.pxw;
                        pos[3] += 1.0f / prop.pxh;

                        pos[4] -= 1.0f / prop.pxw;
                        pos[5] -= 1.0f / prop.pxh;

                        pos[6] += 1.0f / prop.pxw;
                        pos[7] -= 1.0f / prop.pxh;
                    }
                }
            }
        }
    }
}