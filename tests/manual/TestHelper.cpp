// TestHelper.cpp
#include <epoxy/gl.h>

#include "colors.h"
#include "TestHelper.h"

TestHelper::TestHelper(TextManager &textManager, const float gridSpacing, const float gridExtent)
    : m_gridSpacing(gridSpacing),
      m_gridExtent(gridExtent),
      m_mouseX(0.0f),
      m_mouseY(0.0f),
      m_textManager(textManager),
      m_showMouseCoords(false) {
    glClearColor(GL_DARK_BLUE);
    m_mouseText[0] = '\0';
}

float TestHelper::getSpacing() const {
    return m_gridSpacing;
}

void TestHelper::setSpacing(const float spacing) {
    m_gridSpacing = spacing;
}

float TestHelper::getExtent() const {
    return m_gridExtent;
}

void TestHelper::setExtent(const float extent) {
    m_gridExtent = extent;
}

void TestHelper::drawGrid(const GLfloat alpha) const {
    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(0.1f);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

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
    glDisable(GL_BLEND);
}

void TestHelper::drawCenterLines() const {
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

void TestHelper::updateMousePosition(float x, float y) {
    m_mouseX = x;
    m_mouseY = y;
    sprintf(m_mouseText, "%.2f, %.2f", m_mouseX, m_mouseY);
}

void TestHelper::toggleMouseCoordinates(bool show) {
    m_showMouseCoords = show;
}

void TestHelper::drawMouseCoordinates() const {
    if (!m_showMouseCoords) return;

    // Aktuellen Farbwert speichern
    GLfloat oldColor[4];
    glGetFloatv(GL_CURRENT_COLOR, oldColor);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    // Textgröße bestimmen
    const auto fontHeight = m_textManager.getHeight(Fonts::IntroDescription);
    const auto textWidth = fontHeight *8.0f;

    // Position anhand des Quadranten anpassen
    float displayX = m_mouseX;
    float displayY = m_mouseY;

    // X-Position anpassen, damit der Text im Fenster bleibt
    if (m_mouseX + textWidth > m_gridExtent) {
        displayX = m_mouseX - textWidth;
    }

    // Y-Position anpassen, damit der Text im Fenster bleibt
    if (m_mouseY - fontHeight < -m_gridExtent) {
        displayY = m_mouseY + fontHeight;
    }

    // Kleiner Offset für bessere Lesbarkeit
    constexpr float offset = 0.02f;
    displayX += (m_mouseX > 0) ? -offset : offset;
    displayY += (m_mouseY > 0) ? -offset : offset;

    // Text rendern
    m_textManager.write(m_mouseText, Fonts::IntroDescription, false, 1.0f, displayX, displayY);

    // Farbwert wiederherstellen
    glColor4fv(oldColor);
}

void TestHelper::renderInstructions(const float deltaTime, const std::vector<std::string> &instructions) const {
    GLfloat oldColor[4];
    glGetFloatv(GL_CURRENT_COLOR, oldColor);
    glColor4f(GL_ORANGE);
    float yPos = 0.9f;
    constexpr auto currentFont = Fonts::Menu;
    const auto height = m_textManager.getHeight(currentFont);
    const auto offest = height;
    for (const auto &instruction: instructions) {
        m_textManager.write(instruction, currentFont, true, 1.0f, 0.0f, yPos);
        yPos -= height + offest;
    }
    if (m_textManager.getAnnouncementCount() > 0) {
        m_textManager.updateAnnouncements(deltaTime);
        m_textManager.drawAnnouncements(deltaTime);
    }
    glColor4fv(oldColor);
}
