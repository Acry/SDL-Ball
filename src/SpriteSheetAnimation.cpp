#include "SpriteSheetAnimation.h"

SpriteSheetAnimation::SpriteSheetAnimation(const SpriteSheetAnimationProperties &props)
    : animationProperties(props) {
    age = 0.0f;
    direction = props.bidir; // Umbenennung von direction zu bidir
    lastFrame = 0;
    frame = 1;
    playing = props.playing;
    firstFrame = true;
    for (float &v: texturePosition) v = 0.0f;
    calculateTextureCoordinates();
}

void SpriteSheetAnimation::play(const float deltaTime) {
    if (firstFrame) {
        calculateTextureCoordinates();
        firstFrame = false;
    }
    if (playing) {
        age += deltaTime * 1000.0f;

        if (age >= animationProperties.ticks) {
            bool frameChanged = false;
            age = 0.0f;
            frameChanged = true;

            if (!direction) {
                // Vorwärtsanimation
                if (frame == animationProperties.frames) {
                    if (animationProperties.bidir) {
                        // Umbenennung von direction zu bidir
                        direction = true;
                    } else {
                        frame = 1;
                    }
                } else {
                    frame++;
                }
            }

            if (direction) {
                // Rückwärtsanimation
                if (frame == 1) {
                    direction = false;
                    frame = 2;
                } else {
                    frame--;
                }
            }
            calculateTextureCoordinates();
        }
    }
}

void SpriteSheetAnimation::calculateTextureCoordinates() {
    int row = 0;
    Uint32 f = 0;
    int col = 0;

    // Texturkoordinaten für aktuellen Frame berechnen
    for (row = 0; row < animationProperties.rows; row++) {
        for (col = 0; col < animationProperties.cols; col++) {
            f++;
            if (f == frame) {
                // Top left
                texturePosition[0] = (animationProperties.xoffset * static_cast<float>(col));
                texturePosition[1] = (animationProperties.yoffset * static_cast<float>(row));

                // Weitere Texturkoordinaten berechnen
                texturePosition[2] = texturePosition[0] + animationProperties.xoffset;
                texturePosition[3] = texturePosition[1];
                texturePosition[4] = texturePosition[2];
                texturePosition[5] = texturePosition[1] + animationProperties.yoffset;
                texturePosition[6] = texturePosition[0];
                texturePosition[7] = texturePosition[5];

                // Padding anwenden wenn nötig - pxw und pxh müssen über texture-Objekt zugegriffen werden
                if (animationProperties.padding) {
                    const float padOffsetX = 1.0f / animationProperties.pxw;
                    const float padOffsetY = 1.0f / animationProperties.pxh;

                    texturePosition[0] += padOffsetX;
                    texturePosition[1] += padOffsetY;
                    texturePosition[2] -= padOffsetX;
                    texturePosition[3] += padOffsetY;
                    texturePosition[4] -= padOffsetX;
                    texturePosition[5] -= padOffsetY;
                    texturePosition[6] += padOffsetX;
                    texturePosition[7] -= padOffsetY;
                }

                return;
            }
        }
    }
}
