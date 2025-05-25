// MockEventManager.h
#pragma once

#include "EventManager.h"


class MockEventManager : public EventManager {
public:
    GameEvent lastEvent;
    int eventCount = 0;

    void emit(GameEvent event, const EventData &data) override;
};