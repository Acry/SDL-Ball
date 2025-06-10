// InputManager.h
#pragma once
#include <SDL2/SDL_events.h>
#include "IEventManager.h"

class InputManager {
    IEventManager *eventManager;

    ~InputManager();

public:
    explicit InputManager(IEventManager *evtMgr);

    void handleKeyboardEvent(const SDL_KeyboardEvent &keyEvent) const;

    void handleMouseEvent(const SDL_MouseMotionEvent &motion) const;
};
