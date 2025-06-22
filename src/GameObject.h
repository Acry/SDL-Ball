// GameObject.h
#pragma once

#include <epoxy/gl.h>

#include "SpriteSheetAnimation.h"
#include "TextureManager.h"

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

    virtual ~GameObject() = default;

    virtual void init() = 0;

    virtual void draw() const = 0;
};
