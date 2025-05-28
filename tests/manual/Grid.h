// grid.h
#pragma once


class Grid {
    float m_gridSpacing;
    float m_gridExtent;

public:
    // Konstruktor mit optionalem Spacing (Standardwert 0.1f)
    explicit Grid(float gridSpacing = 0.1f, float gridExtent = 1.0f);

    // Getter und Setter für das Spacing
    float getSpacing() const;

    void setSpacing(float spacing);

    // Getter und Setter für die Ausdehnung
    float getExtent() const;

    void setExtent(float extent);

    // Grid zeichnen
    void draw() const;
};
