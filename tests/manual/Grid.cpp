// Grid.cpp
#include <epoxy/gl.h>
#include <cmath>

#include "colors.h"
#include "Grid.h"

Grid::Grid(float gridSpacing, float gridExtent)
    : m_gridSpacing(gridSpacing), m_gridExtent(gridExtent) {
}

float Grid::getSpacing() const {
    return m_gridSpacing;
}

void Grid::setSpacing(float spacing) {
    m_gridSpacing = spacing;
}

float Grid::getExtent() const {
    return m_gridExtent;
}

void Grid::setExtent(float extent) {
    m_gridExtent = extent;
}

void Grid::draw() const {
    // Weißes Gitternetz zeichnen
    glColor4f(GL_WHITE);

    // Vertikale Linien
    glBegin(GL_LINES);
    float x = -m_gridExtent;
    while (x <= m_gridExtent + 0.0001f) {
        glVertex3f(x, -m_gridExtent, 0.0f);
        glVertex3f(x, m_gridExtent, 0.0f);
        x += m_gridSpacing;
    }
    glEnd();

    // Horizontale Linien
    glBegin(GL_LINES);
    float y = -m_gridExtent;
    while (y <= m_gridExtent + 0.0001f) {
        glVertex3f(-m_gridExtent, y, 0.0f);
        glVertex3f(m_gridExtent, y, 0.0f);
        y += m_gridSpacing;
    }
    glEnd();
}
