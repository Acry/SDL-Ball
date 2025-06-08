// EventManager.h
#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>

#include "IEventManager.h"
#include "Paddle.h"

using EventCallback = std::function<void(const EventData &)>;
using LevelEventCallback = std::function<void(const LevelData &)>;
using LevelThemeEventCallback = std::function<void(const LevelThemeData &)>;

class EventManager : public IEventManager{
    struct ListenerEntry {
        void* owner;
        EventCallback callback;
    };

    struct LevelListenerEntry {
        void* owner;
        LevelEventCallback callback;
    };

    struct LevelThemeListenerEntry {
        void *owner;
        LevelThemeEventCallback callback;
    };

    std::unordered_map<GameEvent, std::vector<ListenerEntry>> eventListeners;
    std::unordered_map<GameEvent, std::vector<LevelListenerEntry>> levelEventListeners;
    std::unordered_map<GameEvent, std::vector<LevelThemeListenerEntry> > levelThemeEventListeners;

public:
    void addListener(const GameEvent event, EventCallback callback, void *owner = nullptr) override {
        eventListeners[event].push_back({owner, std::move(callback)});
    }

    void addListener(const GameEvent event, LevelEventCallback callback, void *owner = nullptr) override {
        levelEventListeners[event].push_back({owner, std::move(callback)});
    }

    void addListener(const GameEvent event, LevelThemeEventCallback callback, void *owner = nullptr) override {
        levelThemeEventListeners[event].push_back({owner, std::move(callback)});
    }

    void emit(const GameEvent event, const EventData &data) override {
        auto it = eventListeners.find(event);
        if (it != eventListeners.end()) {
            for (const auto& entry : it->second) {
                entry.callback(data);
            }
        }
    }

    void emit(const GameEvent event, const LevelData &data) override {
        auto it = levelEventListeners.find(event);
        if (it != levelEventListeners.end()) {
            for (const auto& entry : it->second) {
                entry.callback(data);
            }
        }
    }

    void emit(const GameEvent event, const LevelThemeData &data) override {
        auto it = levelThemeEventListeners.find(event);
        if (it != levelThemeEventListeners.end()) {
            for (const auto &entry: it->second) {
                entry.callback(data);
            }
        }
    }

    void removeListener(const GameEvent event, void *owner) override {
        auto it = eventListeners.find(event);
        if (it != eventListeners.end()) {
            auto& listeners = it->second;
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(),
                    [owner](const ListenerEntry& entry) { return entry.owner == owner; }),
                listeners.end()
            );
        }

        auto levelIt = levelEventListeners.find(event);
        if (levelIt != levelEventListeners.end()) {
            auto& listeners = levelIt->second;
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(),
                    [owner](const LevelListenerEntry& entry) { return entry.owner == owner; }),
                listeners.end()
            );
        }
    }
};
