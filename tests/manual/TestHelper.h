// TestHelper.h
#pragma once
#include "colors.h"
class TestHelper {
    float m_gridSpacing;
    float m_gridExtent;

public:
    explicit TestHelper(float gridSpacing = 0.1f, float gridExtent = 1.0f);

    float getSpacing() const;

    void setSpacing(float spacing);

    float getExtent() const;

    void setExtent(float extent);

    void drawGrid(GLfloat alpha = 1.0f) const;

    void drawCenterLines() const;
};
