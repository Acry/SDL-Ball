#include "TextManager.h"

class DebugMouse {
    TextManager& text;
    char tempText[16]{};
    float mouseX;
    float mouseY;

public:
    DebugMouse() : text(TextManager::getInstance()),
                   mouseX(0.0f),
                   mouseY(0.0f) {
        tempText[0] = '\0';
    }

    void update(float x, float y) {
        mouseX = x;
        mouseY = y;
        sprintf(tempText, "%.2f, %.2f", mouseX, mouseY);
        text.write(tempText, FONT_INTRODESCRIPTION, false, 1.0f, mouseX, mouseY);
    }
};
