#pragma once
#include "IInputManager.h"
#include "GameEvents.h"

class KeyboardManager final : public IInputManager {
public:
    explicit KeyboardManager(IEventManager *evtMgr);

    ~KeyboardManager() override;

    void cleanup() override;

    void handleKeyPressedEvent(const KeyboardEventData &data);

    void handleKeyReleasedEvent(const KeyboardEventData &data);

private:
    bool leftKeyPressed = false;
    bool rightKeyPressed = false;
    float keyboardDirection = 0.0f;
    float keyboardMovementSpeed = 20.0f;

    void updatePaddleMovement();
};
