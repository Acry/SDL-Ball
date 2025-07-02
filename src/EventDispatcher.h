#pragma once
#include "IEventManager.h"

class EventDispatcher {
public:
    explicit EventDispatcher(IEventManager *evtMgr);

    ~EventDispatcher();

    [[nodiscard]] bool processEvents();

private:
    IEventManager *eventManager;
    bool running;
};
