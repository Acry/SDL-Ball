// TestHelper.h
#pragma once
#include <vector>

#include "colors.h"
#include "TextManager.h"

class TestHelper {
    float m_gridSpacing;
    float m_gridExtent;

    char m_mouseText[16];
    float m_mouseX;
    float m_mouseY;
    TextManager &m_textManager;

public:
    explicit TestHelper(TextManager &textManager, float gridSpacing = 0.1f, float gridExtent = 1.0f);

    float getSpacing() const;

    void setSpacing(float spacing);

    float getExtent() const;

    void setExtent(float extent);

    void drawGrid(GLfloat alpha = 1.0f) const;

    void drawCenterLines() const;

    void updateMousePosition(float x, float y);

    void toggleMouseCoordinates(bool show);

    void drawMouseCoordinates() const;

    bool m_showMouseCoords;

    void renderInstructions(float deltaTime, const std::vector<std::string> &instructions) const;
};
