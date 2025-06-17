// MouseManager.h
#pragma once
#include "IInputManager.h"

class MouseManager final : public IInputManager {
public:
    explicit MouseManager(IEventManager *evtMgr);

    ~MouseManager() override;

    void cleanup() override;

    void normalizedMouseCoordinates();

    void handleMouseEvent(const MouseEventData &data);

    void handleResizeViewportEvent(const ViewportEventData &data);

    [[nodiscard]] float getNormalizedX() const { return normalizedX; }
    [[nodiscard]] float getNormalizedY() const { return normalizedY; }

private:
    float normalizedX{0.0f};
    float normalizedY{0.0f};
    float mouseX{0.0f};
    float mouseY{0.0f};
    float viewportX{0.0f};
    float viewportY{0.0f};
    float viewportW{0.0f};
    float viewportH{0.0f};
};
