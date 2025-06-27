// EffectDefinitions.h
#pragma once
#include "MathHelper.h"
#include "TextureManager.h"
#include "colors.h"

// Effekttypen
#define FX_SPARKS 0
#define FX_FIRE 1
#define FX_TRANSIT 2
#define FX_PARTICLEFIELD 3
#define FX_TRACER 4

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
    int type = 0;
    int life = 0;
    float size = 0.0f;
    Color color{1.0f, 1.0f, 1.0f, 1.0f};
    bool active = false;
    int effectId = -1;
};

struct ParticleEffectProperties : public BaseEffectProperties {
    int num = 0;
    float speed = 0.0f;
    float spread = 0.0f;
    float gravity = 0.0f;
    TextureResource texture;
    bool collisionDetection = false;
};

struct TransitionEffectProperties : public BaseEffectProperties {
    position rect = {0.0f, 0.0f};
    int transition_half_done = 0;
};
