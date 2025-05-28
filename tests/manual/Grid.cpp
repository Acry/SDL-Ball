// Grid.cpp
#include <epoxy/gl.h>

#include "colors.h"
#include "Grid.h"

Grid::Grid(const float gridSpacing, const float gridExtent)
    : m_gridSpacing(gridSpacing), m_gridExtent(gridExtent) {
}

float Grid::getSpacing() const {
    return m_gridSpacing;
}

void Grid::setSpacing(const float spacing) {
    m_gridSpacing = spacing;
}

float Grid::getExtent() const {
    return m_gridExtent;
}

void Grid::setExtent(const float extent) {
    m_gridExtent = extent;
}

void Grid::draw() const {
    glDisable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(0.1f);
    glColor4f(GL_WHITE);

    // vertical
    glBegin(GL_LINES);
    float x = -m_gridExtent;
    while (x <= m_gridExtent + 0.0001f) {
        glVertex3f(x, -m_gridExtent, 0.0f);
        glVertex3f(x, m_gridExtent, 0.0f);
        x += m_gridSpacing;
    }
    glEnd();

    // horizontal
    glBegin(GL_LINES);
    float y = -m_gridExtent;
    while (y <= m_gridExtent + 0.0001f) {
        glVertex3f(-m_gridExtent, y, 0.0f);
        glVertex3f(m_gridExtent, y, 0.0f);
        y += m_gridSpacing;
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);
}

void Grid::drawCenterLines() const {
    glDisable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);
    glColor4f(GL_FULL_RED);
    glBegin(GL_LINES);

    // horzontal
    glVertex3f(-m_gridExtent, 0.0f, 0.1f);
    glVertex3f(m_gridExtent, 0.0f, 0.1f);

    // vertical
    glVertex3f(0.0f, -m_gridExtent, 0.1f);
    glVertex3f(0.0f, m_gridExtent, 0.1f);

    glEnd();
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);
}
