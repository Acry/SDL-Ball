// Grid.h
#pragma once

class Grid {
    float m_gridSpacing;
    float m_gridExtent;

public:
    explicit Grid(float gridSpacing = 0.1f, float gridExtent = 1.0f);

    float getSpacing() const;

    void setSpacing(float spacing);

    float getExtent() const;

    void setExtent(float extent);

    void draw() const;

    void drawCenterLines() const;
};
