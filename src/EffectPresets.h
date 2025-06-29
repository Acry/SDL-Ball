// EffectPresets.h
#pragma once
#include "EffectDefinitions.h"

namespace EffectPresets {
    inline ParticleEffectProperties createSparksPreset() {
        ParticleEffectProperties props;

        props.type = ParticleEffectType::Sparks;
        props.life = 1500 + randomInt(-200, 200); // Zufällige Lebensdauer
        props.size = 0.02f + randomFloat(0.01f, 0.005f); // Zufällige Größenvariation
        props.color = {
            1.0f,
            0.8f + randomFloat(0.2f, 0.1f), // Gelbton variieren
            0.2f + randomFloat(0.1f, 0.05f),
            1.0f
        };
        props.active = false;
        props.effectId = 1;
        props.count = 200 + randomInt(-20, 20);
        props.speed = 0.7f + randomFloat(0.2f, 0.1f);
        props.spread = 30.0f + randomFloat(10.0f, 5.0f);
        props.gravity = 0.05f + randomFloat(0.02f, 0.01f);
        props.directionAngle = 270.0f + randomFloat(20.0f, 10.0f);
        props.directionVariance = 45.0f + randomFloat(15.0f, 7.5f);

        return props;
    }

    inline ParticleEffectProperties createFirePreset() {
        ParticleEffectProperties props;

        props.type = ParticleEffectType::Fire;
        props.life = 3000 + randomInt(-500, 500);
        props.size = 0.04f + randomFloat(0.02f, 0.01f);
        props.color = {
            1.0f,
            0.3f + randomFloat(0.2f, 0.1f),
            randomFloat(0.2f, 0.0f), // Zufällige Orangetöne
            0.8f + randomFloat(0.2f, 0.1f)
        };
        props.active = false;
        props.effectId = 2;
        props.count = 150 + randomInt(-30, 30);
        props.speed = 0.3f + randomFloat(0.15f, 0.075f);
        props.spread = 120.0f + randomFloat(30.0f, 15.0f);
        props.gravity = -0.02f + randomFloat(0.01f, 0.005f);
        props.directionAngle = 90.0f + randomFloat(20.0f, 10.0f);
        props.directionVariance = 30.0f + randomFloat(10.0f, 5.0f);

        return props;
    }

    inline ParticleEffectProperties createSmokePreset() {
        ParticleEffectProperties props;

        props.type = ParticleEffectType::Smoke;
        props.life = 8000 + randomInt(-800, 800); // Deutlich erhöhte Lebensdauer
        props.size = 0.06f + randomFloat(0.02f, 0.01f);
        float greyValue = 0.7f + randomFloat(0.2f, 0.1f);
        props.color = {greyValue, greyValue, greyValue, 0.3f + randomFloat(0.1f, 0.05f)};
        props.active = false;
        props.effectId = 3;
        props.count = 250 + randomInt(-30, 30);
        props.speed = 0.05f + randomFloat(0.03f, 0.01f);
        props.spread = 60.0f + randomFloat(15.0f, 5.0f);
        props.gravity = -0.09f + randomFloat(0.01f, 0.005f); // Etwas stärkere negative Gravitation
        props.directionAngle = 0.0f;
        props.directionVariance = 5.0f;

        return props;
    }

    inline ParticleEffectProperties createRainPreset() {
        ParticleEffectProperties props;

        props.type = ParticleEffectType::Rain;
        props.life = 2000 + randomInt(-300, 300);
        props.size = 0.03f + randomFloat(0.01f, 0.005f);
        props.color = {
            0.0f,
            0.4f + randomFloat(0.1f, 0.05f),
            0.8f + randomFloat(0.2f, 0.1f),
            0.7f + randomFloat(0.2f, 0.1f)
        };
        props.active = false;
        props.effectId = 4;
        props.count = 180 + randomInt(-30, 30);
        props.speed = 0.5f + randomFloat(0.2f, 0.1f);
        props.spread = 60.0f + randomFloat(15.0f, 7.5f);
        props.gravity = 0.04f + randomFloat(0.02f, 0.01f);
        props.directionAngle = randomFloat(20.0f, 10.0f); // Leicht variierende Richtung
        props.directionVariance = 60.0f + randomFloat(20.0f, 10.0f);

        return props;
    }

    inline ParticleEffectProperties createSparklePreset() {
        ParticleEffectProperties props;

        props.type = ParticleEffectType::SPARKLE;
        props.life = 1000 + randomInt(-200, 200);
        props.size = 0.05f + randomFloat(0.02f, 0.01f);
        props.color = {
            1.0f,
            0.8f + randomFloat(0.2f, 0.1f),
            randomFloat(0.2f, 0.0f),
            1.0f
        };
        props.active = false;
        props.effectId = 6;
        props.count = 500 + randomInt(-50, 50);
        props.speed = 0.4f + randomFloat(0.2f, 0.1f);
        props.spread = 360.0f; // Bleibt konstant für volle Rundumverteilung
        props.gravity = randomFloat(0.02f, 0.01f); // Leichte Zufallsgravitation
        props.directionAngle = randomFloat(360.0f, 0.0f); // Zufällige Startrichtung
        props.directionVariance = 180.0f; // Bleibt konstant für maximale Varianz

        return props;
    }

    inline ParticleEffectProperties createExplosionPreset() {
        ParticleEffectProperties props;

        props.type = ParticleEffectType::Explosion;
        props.life = 1200 + randomInt(-200, 200);
        props.size = 0.05f + randomFloat(0.02f, 0.01f);
        props.color = {
            1.0f,
            0.5f + randomFloat(0.3f, 0.15f),
            randomFloat(0.2f, 0.0f),
            0.9f + randomFloat(0.1f, 0.05f)
        };
        props.active = false;
        props.effectId = 5;
        props.count = 300 + randomInt(-50, 50);
        props.speed = 0.9f + randomFloat(0.3f, 0.15f);
        props.spread = 360.0f; // Bleibt konstant für volle Rundumverteilung
        props.gravity = 0.02f + randomFloat(0.01f, 0.005f);
        props.directionAngle = randomFloat(360.0f, 0.0f); // Zufällige Startrichtung
        props.directionVariance = 180.0f; // Bleibt konstant für maximale Varianz

        return props;
    }

    inline TransitionEffectProperties createFadeInPreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 2000;
        props.color = {0.0f, 0.0f, 0.0f, 1.0f}; // Schwarz
        props.active = true; // Auf true setzen
        props.effectId = -1;
        props.type = TransitionEffectType::FadeIn;
        props.startOpacity = 1.0f;
        props.endOpacity = 0.0f;

        return props;
    }

    inline TransitionEffectProperties createFadeOutPreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 2000;
        props.color = {0.0f, 0.0f, 0.0f, 1.0f}; // Schwarz
        props.active = true; // Auf true setzen
        props.effectId = -1;
        props.type = TransitionEffectType::FadeOut;
        props.startOpacity = 0.0f;
        props.endOpacity = 1.0f;

        return props;
    }

    inline TransitionEffectProperties createCrossFadePreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 2000;
        props.color = {0.5f, 0.5f, 0.5f, 1.0f}; // Grau für Cross-Fade
        props.active = true;
        props.effectId = -1;
        props.type = TransitionEffectType::CrossFade;
        props.startOpacity = 0.0f;
        props.endOpacity = 1.0f;

        return props;
    }

    inline TransitionEffectProperties createSlideInPreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 1500;
        props.color = {0.0f, 0.0f, 0.0f, 1.0f}; // Schwarz
        props.active = true;
        props.effectId = -1;
        props.type = TransitionEffectType::SlideIn;
        props.startOpacity = 1.0f;
        props.endOpacity = 1.0f; // Keine Opazitätsänderung
        props.rect = {-2.0f, 0.0f}; // Startposition außerhalb des Bildschirms

        return props;
    }

    inline TransitionEffectProperties createSlideOutPreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 1500;
        props.color = {0.0f, 0.0f, 0.0f, 1.0f}; // Schwarz
        props.active = true;
        props.effectId = -1;
        props.type = TransitionEffectType::SlideOut;
        props.startOpacity = 1.0f;
        props.endOpacity = 1.0f; // Keine Opazitätsänderung
        props.rect = {0.0f, 0.0f}; // Startposition in der Mitte

        return props;
    }

    inline TransitionEffectProperties createWipeInPreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 1200;
        props.color = {0.0f, 0.0f, 0.0f, 1.0f}; // Schwarz
        props.active = true;
        props.effectId = -1;
        props.type = TransitionEffectType::WipeIn;
        props.startOpacity = 1.0f;
        props.endOpacity = 1.0f; // Keine Opazitätsänderung
        props.rect = {0.0f, 0.0f}; // Wird für die Wipe-Position verwendet

        return props;
    }

    inline TransitionEffectProperties createWipeOutPreset() {
        TransitionEffectProperties props;

        props.category = EffectCategory::Transition;
        props.life = 1200;
        props.color = {0.0f, 0.0f, 0.0f, 1.0f}; // Schwarz
        props.active = true;
        props.effectId = -1;
        props.type = TransitionEffectType::WipeOut;
        props.startOpacity = 1.0f;
        props.endOpacity = 1.0f; // Keine Opazitätsänderung
        props.rect = {-1.0f, 0.0f}; // Startposition für Wipe

        return props;
    }
}
