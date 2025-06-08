// IEventManager.h
#pragma once
#include "GameEvents.h"

class IEventManager {
public:
    virtual ~IEventManager() = default;
    virtual void emit(GameEvent event, const EventData &data) = 0;
    virtual void emit(GameEvent event, const LevelData &data) = 0;
    virtual void emit(GameEvent event, const LevelThemeData &data) = 0;
    virtual void addListener(GameEvent event, EventCallback callback, void *owner = nullptr);
    virtual void addListener(GameEvent event, LevelEventCallback callback, void *owner = nullptr);
    virtual void addListener(GameEvent event, LevelThemeEventCallback callback, void *owner = nullptr);
    virtual void removeListener(GameEvent event, void *owner) = 0;
};
