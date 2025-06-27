# EffectFactory und EffectPresets

## presets

// EffectPresets.h
#pragma once
#include "ParticleEffect.h"

// Vordefinierte Effekt-Parameter als konstante Strukturen
namespace EffectPresets {
// Funkeneffekt (wie bei Flex/Winkelschleifer)
const ParticleEffectProperties Sparks = {
.type = FX_SPARKS,
.life = 1500,
.size = 0.02f,
.color = {1.0f, 0.8f, 0.2f, 1.0f}, // Goldgelb
.active = false,
.effectId = 1,
.num = 200,
.speed = 0.7f,
.spread = 30.0f, // Engerer Ausbreitungswinkel
.gravity = 0.05f,
.directionAngle = 270.0f, // Nach unten gerichtet
.directionVariance = 45.0f // Varianz im Winkel
};

    // Feuereffekt
    const ParticleEffectProperties Fire = {
        .type = FX_FIRE,
        .life = 3000,
        .size = 0.04f,
        .color = {1.0f, 0.3f, 0.0f, 0.8f}, // Orange
        .active = false,
        .effectId = 2,
        .num = 150,
        .speed = 0.3f,
        .spread = 120.0f, // Breiter Winkel
        .gravity = -0.02f, // Negative Gravitation (steigt auf)
        .directionAngle = 90.0f, // Nach oben gerichtet
        .directionVariance = 30.0f
    };
    
    // Raucheffekt
    const ParticleEffectProperties Smoke = {
        .type = FX_SMOKE,
        .life = 5000,
        .size = 0.08f,
        .color = {0.4f, 0.4f, 0.4f, 0.5f}, // Grau, halbtransparent
        .active = false,
        .effectId = 3,
        .num = 100,
        .speed = 0.1f,
        .spread = 90.0f,
        .gravity = -0.01f,
        .directionAngle = 90.0f,
        .directionVariance = 15.0f
    };

    // Wasserspritzer
    const ParticleEffectProperties Water = {
        .type = FX_WATER,
        .life = 2000,
        .size = 0.03f,
        .color = {0.0f, 0.4f, 0.8f, 0.7f}, // Blau, leicht transparent
        .active = false,
        .effectId = 4,
        .num = 180,
        .speed = 0.5f,
        .spread = 60.0f,
        .gravity = 0.04f,
        .directionAngle = 0.0f, // Horizontale Richtung
        .directionVariance = 60.0f
    };

}

## Anpassung der ParticleEffectProperties Struktur

// In EffectDefinitions.h ergänzen:
struct ParticleEffectProperties : public BaseEffectProperties {
int num = 0;
float speed = 0.0f;
float spread = 0.0f;
float gravity = 0.0f;
TextureResource texture;
bool collisionDetection = false;
float directionAngle = 0.0f; // Hauptrichtung des Effekts in Grad
float directionVariance = 0.0f; // Wie stark die Richtung variieren kann

    // Optionale Multi-Color-Unterstützung
    std::vector<Color> colorPalette;
    bool useColorPalette = false;

};

## Generalisierte ParticleEffect-Klasse

// ParticleEffect.h
#pragma once
#include "BaseEffect.h"
#include "Particle.h"

class ParticleEffect : public BaseEffect {
public:
ParticleEffect();
explicit ParticleEffect(const ParticleEffectProperties& preset);
~ParticleEffect() override;

    void init(const position &pos) override;
    void update(float deltaTime) override;
    void draw() override;
    [[nodiscard]] bool isActive() const override;

    // Setter-Methoden
    void setTexture(const TextureResource &tex) { properties.texture = tex; }
    void setColor(const Color &color) { properties.color = color; }
    void setSize(float s) { properties.size = s; }
    void setLife(int life) { properties.life = life; }
    void setParticleCount(int count) { properties.num = count; }
    void setGravity(float gravity) { properties.gravity = gravity; }
    void setSpeed(float speed) { properties.speed = speed; }
    void setSpread(float spread) { properties.spread = spread; }
    void setDirection(float angle) { properties.directionAngle = angle; }
    
    // Multi-Color-Unterstützung
    void setColorPalette(const std::vector<Color>& colors);
    void enableColorPalette(bool enable) { properties.useColorPalette = enable; }

private:
std::vector<Particle> particles;
position spawnPos{};

protected:
ParticleEffectProperties properties;
};

## EffectFactory

// EffectFactory.h
#pragma once
#include "ParticleEffect.h"
#include "TextureManager.h"
#include "EffectPresets.h"

class EffectFactory {
public:
explicit EffectFactory(TextureManager* textureManager)
: m_textureManager(textureManager) {}

    std::unique_ptr<ParticleEffect> createSparkEffect(const position& pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::Sparks);
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);
        effect->init(pos);
        return effect;
    }
    
    std::unique_ptr<ParticleEffect> createFireEffect(const position& pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::Fire);
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);
        
        // Feuereffekt mit Farbpalette für realistischeren Look
        std::vector<Color> fireColors = {
            {1.0f, 0.0f, 0.0f, 1.0f},    // Rot
            {1.0f, 0.3f, 0.0f, 1.0f},    // Orange
            {1.0f, 0.7f, 0.0f, 1.0f},    // Gelb
            {0.7f, 0.7f, 0.7f, 0.5f}     // Rauch
        };
        effect->setColorPalette(fireColors);
        effect->enableColorPalette(true);
        
        effect->init(pos);
        return effect;
    }
    
    // Weitere Factory-Methoden für andere Effekte...

private:
TextureManager* m_textureManager;
};

## Verwendungsbeispiel