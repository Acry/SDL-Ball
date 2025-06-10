// IInputManager.h
#pragma once
#include "IEventManager.h"

class IInputManager {
protected:
    IEventManager *eventManager;

public:
    explicit IInputManager(IEventManager *evtMgr) : eventManager(evtMgr) {
    }

    virtual ~IInputManager() = default;

    virtual void cleanup() = 0;
};
