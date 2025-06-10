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

    virtual void emit(GameEvent event, const CollisionData &data) = 0;

    virtual void emit(GameEvent event, const WindowEventData &data) = 0;

    virtual void emit(GameEvent event, const MouseEventData &data) = 0;

    virtual void emit(GameEvent event, const KeyboardEventData &data) = 0;

    virtual void emit(GameEvent event, const ViewportEventData &data) = 0;

    virtual void emit(GameEvent event, const MouseCoordinatesNormalizedEventData &data) = 0;

    virtual void addListener(GameEvent event, EventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, LevelEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, LevelThemeEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, LevelRequestedEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, ThemeEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, CollisionEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, WindowEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, MouseEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, KeyboardEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, ViewportEventCallback callback, void *owner) = 0;

    virtual void addListener(GameEvent event, MouseCoordinatesNormalizedEventCallback callback, void *owner) = 0;

    virtual void removeListener(GameEvent event, void *owner) = 0;
};
