// GameObject.h
#pragma once

#include <epoxy/gl.h>

#include "SpriteSheetAnimation.h"
#include "TextureManager.h"

class EventManager;

class GameObject {
public:
    explicit GameObject(const texture &tex) {
        textureProperties = tex.textureProperties;
        animProps = tex.animationProperties;
    }

    GLfloat pos_x{0.0f}, pos_y{0.0f};
    GLfloat width{0.0f}, height{0.0f};

    TextureResource textureProperties;
    SpriteSheetAnimationProperties animProps{};

    // In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
    // (0, 1) ------- (1, 1)
    //   |              |
    //   |              |
    //   |              |
    // (0, 0) ------- (1, 0)
    GLfloat uvCoordinates[8]{
        0.0f, 0.0f, // Bottom-left corner (u, v)
        1.0f, 0.0f, // Bottom-right corner (u, v)
        1.0f, 1.0f, // Top-right corner (u, v)
        0.0f, 1.0f // Top-left corner (u, v)
    };

    virtual ~GameObject() = default;

    virtual void init() = 0;

    virtual void draw() const = 0;
};
