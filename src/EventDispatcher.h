#pragma once
#include "IEventManager.h"

class EventDispatcher {
public:
    explicit EventDispatcher(IEventManager *evtMgr) : eventManager(evtMgr) {
    }

    [[nodiscard]] bool processEvents() const;

private:
    IEventManager *eventManager;
};
