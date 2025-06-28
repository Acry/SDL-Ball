// EffectDefinitions.h
#pragma once
#include "MathHelper.h"
#include "TextureManager.h"
#include "colors.h"

enum class ParticleEffectType {
    Sparks = 0,
    Fire = 1,
    Smoke = 2,
    Rain = 3,
    Snow = 4,
    Explosion = 5,
    SPARKLE = 6,
    PARTICLEFIELD = 7,
    Tracer = 8,
    // weitere Partikeleffekte
};

enum class EffectCategory {
    Particle,
    Transition,
    Tracer,
    // weitere Kategorien
};

enum class TransitionEffectType {
    FadeIn = 0,
    FadeOut = 1,
    CrossFade = 2,
    SlideIn = 3,
    SlideOut = 4,
    WipeIn = 5,
    WipeOut = 6,
    // weitere transitions
};

// Effekteigenschaften
#define FX_VAR_TYPE 1
#define FX_VAR_NUM 0
#define FX_VAR_LIFE 2
#define FX_VAR_SPEED 3
#define FX_VAR_SPREAD 4
#define FX_VAR_SIZE 5
#define FX_VAR_GRAVITY 6
#define FX_VAR_COLOR 7
#define FX_VAR_TEXTURE 8
#define FX_VAR_RECTANGLE 9
#define FX_VAR_COLDET 10

// static constexpr int MAX_PARTICLES = 100;

// // Partikel-Arrays
// GLfloat particleX[MAX_PARTICLES]{}; // X-Position der Partikel
// GLfloat particleY[MAX_PARTICLES]{}; // Y-Position der Partikel
// GLfloat particleRed[MAX_PARTICLES]{}; // Rot-Komponente der Partikelfarbe
// GLfloat particleGreen[MAX_PARTICLES]{}; // Grün-Komponente der Partikelfarbe
// GLfloat particleBlue[MAX_PARTICLES]{}; // Blau-Komponente der Partikelfarbe
// GLfloat particleAlpha[MAX_PARTICLES]{}; // Transparenz der Partikel
// GLfloat particleScale[MAX_PARTICLES]{}; // Skalierung der Partikel
// bool particleActive[MAX_PARTICLES]{}; // Status der Partikel (aktiv/inaktiv)
//
// // Aktuelle Eigenschaften
// GLfloat currentRed{1.0f}, currentGreen{0.0f}, currentBlue{0.0f}; // Aktuelle Farbe
// int colorIndex{0}; // Farbindex für Farbrotation
// GLfloat previousX{0.0f}, previousY{0.0f}; // Vorherige Position für Abstandsberechnung
// int maxParticles{MAX_PARTICLES}; // Maximale Anzahl von Partikeln

struct BaseEffectProperties {
    EffectCategory category = EffectCategory::Particle;
    int life = 0;
    float size = 0.0f;
    Color color{1.0f, 1.0f, 1.0f, 1.0f};
    bool active = false;
    int effectId = -1;
};

struct ParticleEffectProperties : public BaseEffectProperties {
    ParticleEffectType type = ParticleEffectType::Sparks;
    int count = 0;
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

struct TransitionEffectProperties : public BaseEffectProperties {
    TransitionEffectType type = TransitionEffectType::FadeIn;
    position rect = {0.0f, 0.0f};
    float startOpacity = 0.0f;
    float endOpacity = 1.0f;
};
