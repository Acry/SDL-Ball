#pragma once
#include <epoxy/gl.h>

#include "MovingObject.h"

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

class Tracer final : public MovingObject {
    GLfloat x[100], y[100]; // Position
    GLfloat r[100], g[100], b[100]; // Farbe
    GLfloat a[100]; // Alpha
    GLfloat s[100]; // Größe
    GLfloat cr{1.0f}, cg{0.0f}, cb{0.0f}; // Aktuelle Farben
    bool active[100];
    int color{0};
    GLfloat lastX{0.0f}, lastY{0.0f}; // Letzte Position
    int len{100};
    bool m_rainbowEnabled{false};

public:
    explicit Tracer(const texture &tex);

    void enableRainbow(bool enable);

    bool isRainbowEnabled() const;

    void init() override;

    void update(float deltaTime) override;

    void draw() const override;

    void setSize(const GLfloat factor);

    void colorRotate(bool explosive, const GLfloat c[]);

    void updatePosition(GLfloat nx, GLfloat ny);

    void updateParticles(float deltaTime);

    void setLastPosition(GLfloat nx, GLfloat ny);

    void rainbowColorUpdate(float deltaTime);
};
