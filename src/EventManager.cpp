// EventManager.cpp
#include "EventManager.h"
#include <algorithm>

void EventManager::addListener(const GameEvent event, EventCallback callback, void *owner) {
    eventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, LevelEventCallback callback, void *owner) {
    levelEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, LevelThemeEventCallback callback, void *owner) {
    levelThemeEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, LevelRequestedEventCallback callback, void *owner) {
    levelRequestedEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, ThemeEventCallback callback, void *owner) {
    themeEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, CollisionEventCallback callback, void *owner) {
    collisionEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, WindowEventCallback callback, void *owner) {
    windowEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, MouseEventCallback callback, void *owner) {
    mouseEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, KeyboardEventCallback callback, void *owner) {
    keyboardEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, ViewportEventCallback callback, void *owner) {
    viewportEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::addListener(const GameEvent event, MouseCoordinatesNormalizedEventCallback callback, void *owner) {
    mouseCoordinatesNormalizedEventListeners[event].push_back({owner, std::move(callback)});
}

void EventManager::emit(const GameEvent event, const EventData &data) {
    auto it = eventListeners.find(event);
    if (it != eventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const LevelData &data) {
    auto it = levelEventListeners.find(event);
    if (it != levelEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const LevelThemeData &data) {
    auto it = levelThemeEventListeners.find(event);
    if (it != levelThemeEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const LevelRequestedData &data) {
    auto it = levelRequestedEventListeners.find(event);
    if (it != levelRequestedEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const ThemeData &data) {
    auto it = themeEventListeners.find(event);
    if (it != themeEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const CollisionData &data) {
    auto it = collisionEventListeners.find(event);
    if (it != collisionEventListeners.end()) {
        for (const auto &entry: it->second) {
            entry.callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const WindowEventData &data) {
    if (const auto it = windowEventListeners.find(event); it != windowEventListeners.end()) {
        for (const auto &[owner, callback]: it->second) {
            callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const KeyboardEventData &data) {
    if (const auto it = keyboardEventListeners.find(event); it != keyboardEventListeners.end()) {
        for (const auto &[owner, callback]: it->second) {
            callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const MouseEventData &data) {
    if (const auto it = mouseEventListeners.find(event); it != mouseEventListeners.end()) {
        for (const auto &[owner, callback]: it->second) {
            callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const ViewportEventData &data) {
    if (const auto it = viewportEventListeners.find(event); it != viewportEventListeners.end()) {
        for (const auto &[owner, callback]: it->second) {
            callback(data);
        }
    }
}

void EventManager::emit(const GameEvent event, const MouseCoordinatesNormalizedEventData &data) {
    if (const auto it = mouseCoordinatesNormalizedEventListeners.find(event);
        it != mouseCoordinatesNormalizedEventListeners.end()) {
        for (const auto &[owner, callback]: it->second) {
            callback(data);
        }
    }
}

void EventManager::removeListener(const GameEvent event, void *owner) {
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
