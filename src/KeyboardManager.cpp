#include "KeyboardManager.h"

#include "config.h"

KeyboardManager::KeyboardManager(IEventManager *evtMgr) : IInputManager(evtMgr) {
    eventManager->addListener(GameEvent::KeyPressed,
                              [this](const KeyboardEventData &data) { handleKeyPressedEvent(data); }, this);
    eventManager->addListener(GameEvent::KeyReleased,
                              [this](const KeyboardEventData &data) { handleKeyReleasedEvent(data); }, this);
}

KeyboardManager::~KeyboardManager() {
    cleanup();
}

void KeyboardManager::cleanup() {
    eventManager->removeListener(GameEvent::KeyPressed, this);
    eventManager->removeListener(GameEvent::KeyReleased, this);
}

void KeyboardManager::handleKeyPressedEvent(const KeyboardEventData &data) {
    if (data.key == SDLK_TAB) {
        eventManager->emit(GameEvent::MenuKeyPressed, data);
        menuInputExclusive = !menuInputExclusive;
        return;
    }

    if (!menuInputExclusive) {
        if (data.key == DEFAULT_KEY_LEFT) {
            leftKeyPressed = true;
            updatePaddleMovement();
        } else if (data.key == DEFAULT_KEY_RIGHT) {
            rightKeyPressed = true;
            updatePaddleMovement();
        }
    }
}

void KeyboardManager::handleKeyReleasedEvent(const KeyboardEventData &data) {
    if (menuInputExclusive) {
        if (data.key == SDLK_RETURN || data.key == SDLK_KP_ENTER ||
            data.key == SDLK_UP || data.key == SDLK_DOWN ||
            data.key == SDLK_LEFT || data.key == SDLK_RIGHT) {
            eventManager->emit(GameEvent::MenuKeyReleased, data);
        }
        return;
    }
    if (data.key == DEFAULT_KEY_LEFT) {
        leftKeyPressed = false;
        updatePaddleMovement();
    } else if (data.key == DEFAULT_KEY_RIGHT) {
        rightKeyPressed = false;
        updatePaddleMovement();
    }
}

void KeyboardManager::updatePaddleMovement() {
    if (leftKeyPressed && !rightKeyPressed) {
        keyboardDirection = -1.0f;
    } else if (rightKeyPressed && !leftKeyPressed) {
        keyboardDirection = 1.0f;
    } else {
        keyboardDirection = 0.0f;
    }
    KeyboardMoveEventData moveData;
    moveData.direction = keyboardDirection;
    moveData.speed = keyboardMovementSpeed;
    eventManager->emit(GameEvent::KeyboardPaddleMove, moveData);
}
