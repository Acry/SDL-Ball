// EffectFactory.h
#pragma once
#include "EffectPresets.h"
#include "ParticleEffect.h"
#include "TextureManager.h"
#include "TransitionEffect.h"

class EffectFactory {
public:
    explicit EffectFactory(TextureManager *textureManager)
        : m_textureManager(textureManager) {
    }

    std::unique_ptr<ParticleEffect> createSparkEffect(const position &pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::createSparksPreset());
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);
        effect->init(pos);
        return effect;
    }

    std::unique_ptr<ParticleEffect> createFireEffect(const position &pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::createFirePreset());
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);

        // Feuereffekt mit Farbpalette
        std::vector<Color> fireColors = {
            {1.0f, 0.0f, 0.0f, 1.0f}, // Rot
            {1.0f, 0.3f, 0.0f, 1.0f}, // Orange
            {1.0f, 0.7f, 0.0f, 1.0f}, // Gelb
            {0.7f, 0.7f, 0.7f, 0.5f} // Rauch
        };
        effect->setColorPalette(fireColors);
        effect->enableColorPalette(true);

        effect->init(pos);
        return effect;
    }

    std::unique_ptr<ParticleEffect> createSmokeEffect(const position &pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::createSmokePreset());
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);

        // Raucheffekt mit Farbpalette für realistischeren Eindruck
        std::vector<Color> smokeColors = {
            {0.5f, 0.5f, 0.5f, 0.7f}, // Dunkler Rauch
            {0.6f, 0.6f, 0.6f, 0.5f}, // Mittlerer Rauch
            {0.7f, 0.7f, 0.7f, 0.3f}, // Hellerer Rauch
            {0.8f, 0.8f, 0.8f, 0.2f} // Sehr heller, fast transparenter Rauch
        };
        effect->setColorPalette(smokeColors);
        effect->enableColorPalette(true);

        effect->init(pos);
        return effect;
    }

    std::unique_ptr<ParticleEffect> createRainEffect(const position &pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::createRainPreset());
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);

        // Optional: Farbpalette für Regen mit verschiedenen Blautönen
        std::vector<Color> rainColors = {
            {0.0f, 0.3f, 0.7f, 0.8f},
            {0.1f, 0.4f, 0.8f, 0.7f},
            {0.0f, 0.5f, 0.9f, 0.6f}
        };
        effect->setColorPalette(rainColors);
        effect->enableColorPalette(true);

        effect->init(pos);
        return effect;
    }

    std::unique_ptr<ParticleEffect> createExplosionEffect(const position &pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::createExplosionPreset());
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);

        // Farbpalette für realistische Explosion
        std::vector<Color> explosionColors = {
            {1.0f, 0.8f, 0.0f, 1.0f}, // Helles Gelb
            {1.0f, 0.5f, 0.0f, 0.9f}, // Orange
            {1.0f, 0.2f, 0.0f, 0.8f}, // Rot
            {0.7f, 0.7f, 0.7f, 0.6f}, // Grau für Rauch
            {0.5f, 0.5f, 0.5f, 0.4f} // Dunklerer Rauch
        };
        effect->setColorPalette(explosionColors);
        effect->enableColorPalette(true);

        effect->init(pos);
        return effect;
    }

    std::unique_ptr<ParticleEffect> createSparkleEffect(const position &pos) {
        auto effect = std::make_unique<ParticleEffect>(EffectPresets::createSparklePreset());
        effect->setTexture(m_textureManager->getEffectTexture(EffectTexture::Particle)->textureProperties);
        effect->setColor(Color{GL_DEEP_ORANGE});
        effect->init(pos);
        return effect;
    }

    std::unique_ptr<TransitionEffect> createFadeInEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::FadeIn);
        effect->setLife(1000); // 1 Sekunde
        effect->setColor({0.0f, 0.0f, 0.0f, 1.0f}); // Schwarzer Fade
        effect->init({0.0f, 0.0f});
        return effect;
    }

    std::unique_ptr<TransitionEffect> createFadeOutEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::FadeOut);
        effect->setLife(1000); // 1 Sekunde
        effect->setColor({0.0f, 0.0f, 0.0f, 1.0f}); // Schwarzer Fade
        effect->init({0.0f, 0.0f});
        return effect;
    }

    std::unique_ptr<TransitionEffect> createCrossFadeEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::CrossFade);
        effect->setLife(2000);
        effect->setColor({0.5f, 0.5f, 0.5f, 1.0f}); // Grau für CrossFade
        effect->init({0.0f, 0.0f});
        return effect;
    }

    std::unique_ptr<TransitionEffect> createSlideInEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::SlideIn);
        effect->setLife(1500);
        effect->setColor({0.0f, 0.0f, 0.0f, 1.0f}); // Schwarz
        effect->init({0.0f, 0.0f});
        return effect;
    }

    std::unique_ptr<TransitionEffect> createSlideOutEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::SlideOut);
        effect->setLife(1500);
        effect->setColor({0.0f, 0.0f, 0.0f, 1.0f}); // Schwarz
        effect->init({0.0f, 0.0f});
        return effect;
    }

    std::unique_ptr<TransitionEffect> createWipeInEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::WipeIn);
        effect->setLife(1200);
        effect->setColor({0.0f, 0.0f, 0.0f, 1.0f}); // Schwarz
        effect->init({0.0f, 0.0f});
        return effect;
    }

    std::unique_ptr<TransitionEffect> createWipeOutEffect() {
        auto effect = std::make_unique<TransitionEffect>();
        effect->setType(TransitionEffectType::WipeOut);
        effect->setLife(1200);
        effect->setColor({0.0f, 0.0f, 0.0f, 1.0f}); // Schwarz
        effect->init({0.0f, 0.0f});
        return effect;
    }

private:
    TextureManager *m_textureManager;
};
