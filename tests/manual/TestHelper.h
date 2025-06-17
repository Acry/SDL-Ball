// TestHelper.h
#pragma once
#include <filesystem>
#include <vector>
#include "IEventManager.h"
#include "TextManager.h"

class TestHelper {
    float m_gridSpacing;
    float m_gridExtent;

    char m_mouseText[64]{};

    TextManager &m_textManager;
    IEventManager *m_eventManager;
    const std::filesystem::path screenshotPath = "./screenshots/";

public:
    float m_mouseX;
    float m_mouseY;

    explicit TestHelper(TextManager &textManager, IEventManager *eventManager, float gridSpacing = 0.1f,
                        float gridExtent = 1.0f);

    virtual ~TestHelper();

    [[nodiscard]] float getSpacing() const;

    void setSpacing(float spacing);

    [[nodiscard]] float getExtent() const;

    void setExtent(float extent);

    void drawGrid(GLfloat alpha = 1.0f) const;

    void drawCenterLines() const;

    void updateMousePosition(float x, float y);

    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data);

    void toggleMouseCoordinates(bool show);

    void drawMouseCoordinates() const;

    virtual void handleKeyPress(const KeyboardEventData &data);

    virtual void handleMouseButton(const MouseEventData &data);

    virtual void MouseWheelScrolled(const MouseEventData &data);

    [[nodiscard]] bool screenshot() const;

    bool m_showMouseCoords;

    void renderInstructions(float deltaTime, const std::vector<std::string> &instructions) const;
};
