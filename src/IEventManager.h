// IEventManager.h
#pragma once
#include "GameEvents.h"

class IEventManager {
public:
    virtual ~IEventManager() = default;

    virtual void emit(GameEvent event, const EventData &data) = 0;

    virtual void emit(GameEvent event, const LevelData &data) = 0;

    virtual void emit(GameEvent event, const LevelThemeData &data) = 0;

    virtual void emit(GameEvent event, const LevelRequestedData &data) = 0;

    virtual void emit(GameEvent event, const ThemeData &data) = 0;

    virtual void addListener(GameEvent event, EventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, LevelEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, LevelThemeEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, LevelRequestedEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, ThemeEventCallback callback, void *owner) = 0;

    virtual void removeListener(GameEvent event, void *owner) = 0;
};
