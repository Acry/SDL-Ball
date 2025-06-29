// TransitionEffect.h
#pragma once
#include "BaseEffect.h"

class TransitionEffect final : public BaseEffect {
public:
    TransitionEffect();

    ~TransitionEffect() override = default;

    void init(const position &pos) override;

    void update(float deltaTime) override;

    void draw() override;

    [[nodiscard]] bool isActive() const override;

    // Setter-Methoden
    void setColor(const Color &color) { properties.color = color; }
    void setLife(int life) { properties.life = life; }
    void setType(TransitionEffectType type) { properties.type = type; }

    void activate() {
        properties.active = true;
        age = 0;
        opacity = properties.startOpacity;
    }

    void setTexture(GLuint textureId) { currentTextureId = textureId; }

    void setTextures(GLuint currentTexId, GLuint nextTexId) {
        currentTextureId = currentTexId;
        nextTextureId = nextTexId;
    }

private:
    TransitionEffectProperties properties;
    float opacity{0.0f};
    int age{0};
    GLuint currentTextureId{0}; // Neue Variable für die Textur-ID
    GLuint nextTextureId{0}; // Nächste Szene
    void renderTexture(GLuint textureId, float alpha);
};
