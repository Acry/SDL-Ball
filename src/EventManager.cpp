// EventManager.cpp
#include "EventManager.h"
#include <algorithm>

void EventManager::addListener(GameEvent event, EventCallback callback, void *owner) {
    eventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(GameEvent event, LevelEventCallback callback, void *owner) {
    levelEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(GameEvent event, LevelThemeEventCallback callback, void *owner) {
    levelThemeEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(GameEvent event, LevelRequestedEventCallback callback, void *owner) {
    levelRequestedEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(GameEvent event, ThemeEventCallback callback, void *owner) {
    themeEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::emit(GameEvent event, const EventData &data) {
    auto it = eventListeners.find(event);
    if (it != eventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(GameEvent event, const LevelData &data) {
    auto it = levelEventListeners.find(event);
    if (it != levelEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(GameEvent event, const LevelThemeData &data) {
    auto it = levelThemeEventListeners.find(event);
    if (it != levelThemeEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(GameEvent event, const LevelRequestedData &data) {
    auto it = levelRequestedEventListeners.find(event);
    if (it != levelRequestedEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(GameEvent event, const ThemeData &data) {
    auto it = themeEventListeners.find(event);
    if (it != themeEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::removeListener(GameEvent event, void *owner) {
    auto removeFromMap = [owner](auto &map, const GameEvent &evt) {
        auto it = map.find(evt);
        if (it != map.end()) {
            auto &listeners = it->second;
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(),
                               [owner](const auto &entry) { return entry.owner == owner; }),
                listeners.end()
            );
        }
    };

    removeFromMap(eventListeners, event);
    removeFromMap(levelEventListeners, event);
    removeFromMap(levelThemeEventListeners, event);
    removeFromMap(levelRequestedEventListeners, event);
    removeFromMap(themeEventListeners, event);
}
