// EventManager.h
#pragma once
#include <vector>
#include <unordered_map>
#include "IEventManager.h"

class EventManager : public IEventManager {
    struct ListenerEntry {
        void *owner;
        EventCallback callback;
    };

    struct LevelListenerEntry {
        void *owner;
        LevelEventCallback callback;
    };

    struct LevelThemeListenerEntry {
        void *owner;
        LevelThemeEventCallback callback;
    };

    struct LevelRequestedListenerEntry {
        void *owner;
        LevelRequestedEventCallback callback;
    };

    struct ThemeListenerEntry {
        void *owner;
        ThemeEventCallback callback;
    };

    std::unordered_map<GameEvent, std::vector<ListenerEntry> > eventListeners;
    std::unordered_map<GameEvent, std::vector<LevelListenerEntry> > levelEventListeners;
    std::unordered_map<GameEvent, std::vector<LevelThemeListenerEntry> > levelThemeEventListeners;
    std::unordered_map<GameEvent, std::vector<LevelRequestedListenerEntry> > levelRequestedEventListeners;
    std::unordered_map<GameEvent, std::vector<ThemeListenerEntry> > themeEventListeners;

public:
    void addListener(GameEvent event, EventCallback callback, void *owner = nullptr) override;

    void addListener(GameEvent event, LevelEventCallback callback, void *owner = nullptr) override;

    void addListener(GameEvent event, LevelThemeEventCallback callback, void *owner = nullptr) override;

    void addListener(GameEvent event, LevelRequestedEventCallback callback, void *owner = nullptr) override;

    void addListener(GameEvent event, ThemeEventCallback callback, void *owner = nullptr) override;

    void emit(GameEvent event, const EventData &data) override;

    void emit(GameEvent event, const LevelData &data) override;

    void emit(GameEvent event, const LevelThemeData &data) override;

    void emit(GameEvent event, const LevelRequestedData &data) override;

    void emit(GameEvent event, const ThemeData &data) override;

    void removeListener(GameEvent event, void *owner) override;
};
