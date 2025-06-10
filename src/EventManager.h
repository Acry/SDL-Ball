// EventManager.h
#pragma once
#include <vector>
#include <unordered_map>
#include "IEventManager.h"

class EventManager : public IEventManager {
    template<typename CallbackType>
    struct ListenerEntryBase {
        void *owner{nullptr};
        CallbackType callback{};
    };

    using ListenerEntry = ListenerEntryBase<EventCallback>;
    using LevelListenerEntry = ListenerEntryBase<LevelEventCallback>;
    using LevelThemeListenerEntry = ListenerEntryBase<LevelThemeEventCallback>;
    using LevelRequestedListenerEntry = ListenerEntryBase<LevelRequestedEventCallback>;
    using ThemeListenerEntry = ListenerEntryBase<ThemeEventCallback>;
    using CollisionEventListenerEntry = ListenerEntryBase<CollisionEventCallback>;
    using WindowEventListenerEntry = ListenerEntryBase<WindowEventCallback>;
    using KeyboardEventListenerEntry = ListenerEntryBase<KeyboardEventCallback>;
    using MouseEventListenerEntry = ListenerEntryBase<MouseEventCallback>;
    using ViewportEventListenerEntry = ListenerEntryBase<ViewportEventCallback>;
    using MouseCoordinatesNormalizedEventListenerEntry = ListenerEntryBase<MouseCoordinatesNormalizedEventCallback>;

    std::unordered_map<GameEvent, std::vector<ListenerEntry> > eventListeners;
    std::unordered_map<GameEvent, std::vector<LevelListenerEntry> > levelEventListeners;
    std::unordered_map<GameEvent, std::vector<LevelThemeListenerEntry> > levelThemeEventListeners;
    std::unordered_map<GameEvent, std::vector<LevelRequestedListenerEntry> > levelRequestedEventListeners;
    std::unordered_map<GameEvent, std::vector<ThemeListenerEntry> > themeEventListeners;
    std::unordered_map<GameEvent, std::vector<CollisionEventListenerEntry> > collisionEventListeners;
    std::unordered_map<GameEvent, std::vector<WindowEventListenerEntry> > windowEventListeners;
    std::unordered_map<GameEvent, std::vector<MouseEventListenerEntry> > mouseEventListeners;
    std::unordered_map<GameEvent, std::vector<KeyboardEventListenerEntry> > keyboardEventListeners;
    std::unordered_map<GameEvent, std::vector<ViewportEventListenerEntry> > viewportEventListeners;
    std::unordered_map<GameEvent, std::vector<MouseCoordinatesNormalizedEventListenerEntry> >
    mouseCoordinatesNormalizedEventListeners;

public:
    void addListener(GameEvent event, EventCallback callback, void *owner) override;

    void addListener(GameEvent event, LevelEventCallback callback, void *owner) override;

    void addListener(GameEvent event, LevelThemeEventCallback callback, void *owner) override;

    void addListener(GameEvent event, LevelRequestedEventCallback callback, void *owner) override;

    void addListener(GameEvent event, ThemeEventCallback callback, void *owner) override;

    void addListener(GameEvent event, CollisionEventCallback callback, void *owner) override;

    void addListener(GameEvent event, WindowEventCallback callback, void *owner) override;

    void addListener(GameEvent event, MouseEventCallback callback, void *owner) override;

    void addListener(GameEvent event, KeyboardEventCallback callback, void *owner) override;

    void addListener(GameEvent event, ViewportEventCallback callback, void *owner) override;

    void addListener(GameEvent event, MouseCoordinatesNormalizedEventCallback callback, void *owner) override;

    void emit(GameEvent event, const EventData &data) override;

    void emit(GameEvent event, const LevelData &data) override;

    void emit(GameEvent event, const LevelThemeData &data) override;

    void emit(GameEvent event, const LevelRequestedData &data) override;

    void emit(GameEvent event, const ThemeData &data) override;

    void emit(GameEvent event, const CollisionData &data) override;

    void emit(GameEvent event, const WindowEventData &data) override;

    void emit(GameEvent event, const MouseEventData &data) override;

    void emit(GameEvent event, const KeyboardEventData &data) override;

    void emit(GameEvent event, const ViewportEventData &data) override;

    void emit(GameEvent event, const MouseCoordinatesNormalizedEventData &data) override;

    void removeListener(GameEvent event, void *owner) override;
};
