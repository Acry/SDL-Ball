#include "KeyboardManager.h"

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
    // Tastendruck verarbeiten
}

void KeyboardManager::handleKeyReleasedEvent(const KeyboardEventData &data) {
    // Tastenloslassen verarbeiten
}
