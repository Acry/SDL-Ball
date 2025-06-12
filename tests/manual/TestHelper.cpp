// TestHelper.cpp
#include <epoxy/gl.h>
#include <SDL_log.h>

#include "colors.h"
#include "TestHelper.h"

TestHelper::TestHelper(TextManager &textManager, IEventManager *eventManager, const float gridSpacing,
                       const float gridExtent)
    : m_gridSpacing(gridSpacing),
      m_gridExtent(gridExtent),
      m_mouseX(0.0f),
      m_mouseY(0.0f),
      m_textManager(textManager), m_eventManager(eventManager),
      m_showMouseCoords(false) {
    glClearColor(GL_LIGHT_BLUE);
    m_mouseText[0] = '\0';

    m_eventManager->addListener(GameEvent::MouseCoordinatesNormalized,
                                [this](const MouseCoordinatesNormalizedEventData &data) {
                                    this->handleMouseCoordinatesNormalized(data);
                                }, this);

    m_eventManager->addListener(GameEvent::KeyPressed,
                                [this](const KeyboardEventData &data) {
                                    this->handleKeyPress(data);
                                }, this);
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

    // horizontal
    glVertex3f(-m_gridExtent, 0.0f, 0.1f);
    glVertex3f(m_gridExtent, 0.0f, 0.1f);

    // vertical
    glVertex3f(0.0f, -m_gridExtent, 0.1f);
    glVertex3f(0.0f, m_gridExtent, 0.1f);

    glEnd();
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);
}

void TestHelper::updateMousePosition(const float x, const float y) {
    m_mouseX = x;
    m_mouseY = y;
    snprintf(m_mouseText, sizeof(m_mouseText), "x: %.2f, y: %.2f", m_mouseX, m_mouseY);
    // SDL_Log("Mouse Position: (%.2f, %.2f)", m_mouseX, m_mouseY);
}

void TestHelper::handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) {
    updateMousePosition(data.x, data.y);
}

void TestHelper::toggleMouseCoordinates(const bool show) {
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
    const auto textWidth = fontHeight * 8.0f;

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
    glColor4f(GL_BRIGHT_ORANGE);
    float yPos = 0.95f;
    constexpr auto currentFont = Fonts::Menu;
    const auto height = m_textManager.getHeight(currentFont);
    for (const auto &instruction: instructions) {
        const auto offest = height * 0.7f; // 90% der Schriftgröße als Offset
        m_textManager.write(instruction, currentFont, true, 0.7f, 0.0f, yPos);
        yPos -= offest;
    }
    if (m_textManager.getAnnouncementCount() > 0) {
        m_textManager.updateAnnouncements(deltaTime);
        m_textManager.drawAnnouncements(deltaTime);
    }
    glColor4fv(oldColor);
}

TestHelper::~TestHelper() {
    m_eventManager->removeListener(GameEvent::MouseCoordinatesNormalized, this);
    m_eventManager->removeListener(GameEvent::KeyPressed, this);
}

void TestHelper::handleKeyPress(const KeyboardEventData &data) {
    switch (data.key) {
        case SDLK_m:
            m_showMouseCoords = !m_showMouseCoords;
            break;
        case SDLK_s:
            if (!std::filesystem::exists(screenshotPath)) {
                std::filesystem::create_directories(screenshotPath);
            }
            if (screenshot()) {
                m_textManager.addAnnouncement("Screenshot saved.", 3000, Fonts::AnnounceGood);
            } else {
                m_textManager.addAnnouncement("Screenshot not created.", 3000, Fonts::AnnounceBad);
            }
            break;
        // Weitere Tastenbelegungen hier ergänzen
        default:
            break;
    }
}

bool TestHelper::screenshot() const {
    static constexpr size_t MAX_FILENAME = 256;
    static constexpr size_t TGA_HEADER_SIZE = 12;
    static constexpr size_t TGA_INFO_SIZE = 6;
    static constexpr size_t CHANNELS = 3; // BGR

    char fileName[MAX_FILENAME];
    int fileIndex = 0;

    // Finde freien Dateinamen
    while (fileIndex < 9999) {
        const int result = snprintf(fileName, MAX_FILENAME, "%s/sdl-ball-test-%04d.tga",
                                    screenshotPath.c_str(), fileIndex);

        if (result < 0 || static_cast<size_t>(result) >= MAX_FILENAME) {
            SDL_Log("Filename too long");
            return false;
        }

        FILE *test = fopen(fileName, "rb");
        if (!test) break;
        fclose(test);
        fileIndex++;
    }

    if (fileIndex == 9999) {
        SDL_Log("No free filename found");
        return false;
    }

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const GLint vpX = viewport[0];
    const GLint vpY = viewport[1];
    const GLint vpW = viewport[2];
    const GLint vpH = viewport[3];

    // Alloziere Pixel Buffer
    const size_t pixelCount = vpW * vpH * CHANNELS;
    const auto pixels = std::make_unique<GLubyte[]>(pixelCount);
    if (!pixels) {
        SDL_Log("Memory allocation failed");
        return false;
    }

    // Öffne Ausgabedatei
    FILE *outFile = fopen(fileName, "wb");
    if (!outFile) {
        SDL_Log("Could not create file '%s'", fileName);
        return false;
    }

    // Lese Framebuffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(vpX, vpY, vpW, vpH, GL_BGR, GL_UNSIGNED_BYTE, pixels.get());

    if (glGetError() != GL_NO_ERROR) {
        SDL_Log("Failed reading OpenGL framebuffer.");
        fclose(outFile);
        return false;
    }

    // Schreibe TGA Header
    const unsigned char tgaHeader[TGA_HEADER_SIZE] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const unsigned char tgaInfo[TGA_INFO_SIZE] = {
        static_cast<unsigned char>(vpW & 0xFF),
        static_cast<unsigned char>(vpW >> 8 & 0xFF),
        static_cast<unsigned char>(vpH & 0xFF),
        static_cast<unsigned char>(vpH >> 8 & 0xFF),
        24, 0
    };

    if (fwrite(tgaHeader, 1, TGA_HEADER_SIZE, outFile) != TGA_HEADER_SIZE ||
        fwrite(tgaInfo, 1, TGA_INFO_SIZE, outFile) != TGA_INFO_SIZE ||
        fwrite(pixels.get(), 1, pixelCount, outFile) != pixelCount) {
        SDL_Log("Failed writing TGA file");
        fclose(outFile);
        return false;
    }

    fclose(outFile);
    //SDL_Log("Screenshot saved as '%s'", fileName);
    return true;
}
