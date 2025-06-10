// InputManager.cpp
#include "InputManager.h"

InputManager::InputManager(IEventManager *evtMgr)
    : eventManager(evtMgr) {
    eventManager->addListener(GameEvent::KeyPressed,
                              [this](const KeyboardEventData &data) {
                              }, this);

    eventManager->addListener(GameEvent::MouseMoved,
                              [this](const MouseEventData &data) {
                              }, this);
}

void InputManager::handleKeyboardEvent(const SDL_KeyboardEvent &keyEvent) const {
    const KeyboardEventData data{keyEvent.keysym.sym, keyEvent.type == SDL_KEYDOWN};
    eventManager->emit(keyEvent.type == SDL_KEYDOWN ? GameEvent::KeyPressed : GameEvent::KeyReleased, data);
}

void InputManager::handleMouseEvent(const SDL_MouseMotionEvent &motion) const {
    const MouseEventData data{};
    eventManager->emit(GameEvent::MouseMoved, data);
}

InputManager::~InputManager() {
    eventManager->removeListener(GameEvent::KeyPressed, this);
    eventManager->removeListener(GameEvent::MouseMoved, this);
}
