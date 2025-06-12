#include <SDL2/SDL_log.h>

#include "SpriteSheetAnimation.h"

SpriteSheetAnimation::SpriteSheetAnimation(const SpriteSheetAnimationProperties &props)
    : animationProperties(props) {
    currentFrameAge = 0.0f;
    currentDirection = props.bidir;
    currentFrame = 0;
    isPlaying = props.playing;
    for (float &v: currentTexturePosition) v = 0.0f;
    calculateTextureCoordinates();
}

void SpriteSheetAnimation::play(const float deltaTime) {
    if (!isPlaying) return;
    currentFrameAge += deltaTime * 1000.0f;
    if (currentFrameAge < animationProperties.ticks) return;
    currentFrameAge -= animationProperties.ticks;

    const Uint32 totalFrames = animationProperties.frames;
    if (totalFrames == 0) return;

    if (animationProperties.bidir) {
        if (currentDirection) {
            ++currentFrame;
            if (currentFrame + 1 >= totalFrames) {
                currentFrame = totalFrames - 1;
                currentDirection = false;
            }
        } else {
            --currentFrame;
            if (currentFrame == 0) {
                if (animationProperties.playOnce) {
                    isPlaying = false;
                } else {
                    currentDirection = true;
                }
            }
        }
    } else {
        ++currentFrame;
        if (currentFrame >= totalFrames) {
            if (!animationProperties.playOnce) {
                currentFrame = 0;
            } else {
                currentFrame = totalFrames - 1;
                isPlaying = false;
            }
        }
    }
    calculateTextureCoordinates();
}

void SpriteSheetAnimation::calculateTextureCoordinates() {
    const int cols = animationProperties.cols;
    const int rows = animationProperties.rows;
    if (cols <= 0 || rows <= 0) {
        SDL_Log("Error: cols=%d, rows=%d müssen >0 sein", cols, rows);
        return;
    }

    const Uint32 total = animationProperties.frames;
    const Uint32 f = currentFrame >= total ? total - 1 : currentFrame;

    const Uint32 col = f % cols;
    Uint32 row = f / cols;

    // start top left
    row = rows - 1 - row;

    const float x0 = animationProperties.xoffset * col;
    const float y0 = animationProperties.yoffset * row;
    const float x1 = x0 + animationProperties.xoffset;
    const float y1 = y0 + animationProperties.yoffset;

    currentTexturePosition[0] = x0;
    currentTexturePosition[1] = y0;
    currentTexturePosition[2] = x1;
    currentTexturePosition[3] = y0;
    currentTexturePosition[4] = x1;
    currentTexturePosition[5] = y1;
    currentTexturePosition[6] = x0;
    currentTexturePosition[7] = y1;

    if (animationProperties.padding) {
        const float px = 1.0f / animationProperties.pxw;
        const float py = 1.0f / animationProperties.pxh;
        currentTexturePosition[0] += px;
        currentTexturePosition[1] += py;
        currentTexturePosition[2] -= px;
        currentTexturePosition[3] += py;
        currentTexturePosition[4] -= px;
        currentTexturePosition[5] -= py;
        currentTexturePosition[6] += px;
        currentTexturePosition[7] -= py;
    }
}
