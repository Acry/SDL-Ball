// IInputManager.h
#pragma once
#include "IEventManager.h"

class IInputManager {
protected:
    IEventManager *eventManager;
    bool menuInputExclusive = false;

public:
    explicit IInputManager(IEventManager *evtMgr) : eventManager(evtMgr) {
    }

    virtual ~IInputManager() = default;

    virtual void cleanup() = 0;
};
