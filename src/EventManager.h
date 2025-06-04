// EventManager.h
#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>

#include "LevelManager.h"
#include "Paddle.h"


enum class GameEvent {
    BallHitLeftBorder, // Index 0
    BallHitRightBorder, // Index 1
    BallHitTopBorder, // Index 2
    BallLost, // Index 3
    BallHitPaddle, // Index 4
    BrickDestroyed, // Index 5
    PowerUpCollected, // Index 6
    BallHitBrick, // Index 7
    PaddleHitLeftBorder, // Index 8
    PaddleHitRightBorder, // Index 9

    // Ball-Events für Tracer
    BallCreated,
    BallMoved,
    BallDestroyed,

    // Neue Tracer-bezogene Events
    CreateTracer,
    TracerCreated,
    UpdateTracer,
    UpdateTracerColor,
    UpdateTracerSize,
    RemoveTracer,

    PaddleCreated,
    PaddleMoved,
    PaddleDestroyed,

    LevelChanged,
    LevelThemeChanged,
};

struct EventData {
    float posX{0};
    float posY{0};
    int soundID{-1};
    GameObject *sender{nullptr};
    const GameObject *target{nullptr};
    // ---snip--- TODO/FIXME
    int points{0};
};

struct LevelEventData {
    size_t currentlevel{};
    BrickData brickData{};
    PowerupData powerupData{};
};

struct LevelThemeData {
    size_t maxLevel{};
};

using EventCallback = std::function<void(const EventData &)>;
using LevelEventCallback = std::function<void(const LevelEventData &)>;
using LevelThemeEventCallback = std::function<void(const LevelThemeData &)>;

class EventManager {
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
    void addListener(const GameEvent event, EventCallback callback, void *owner = nullptr) {
        eventListeners[event].push_back({owner, std::move(callback)});
    }

    void addListener(const GameEvent event, LevelEventCallback callback, void *owner = nullptr) {
        levelEventListeners[event].push_back({owner, std::move(callback)});
    }

    void addListener(const GameEvent event, LevelThemeEventCallback callback, void *owner = nullptr) {
        levelThemeEventListeners[event].push_back({owner, std::move(callback)});
    }

    void emit(const GameEvent event, const EventData &data) {
        auto it = eventListeners.find(event);
        if (it != eventListeners.end()) {
            for (const auto& entry : it->second) {
                entry.callback(data);
            }
        }
    }

    void emit(const GameEvent event, const LevelEventData &data) {
        auto it = levelEventListeners.find(event);
        if (it != levelEventListeners.end()) {
            for (const auto& entry : it->second) {
                entry.callback(data);
            }
        }
    }

    void emit(const GameEvent event, const LevelThemeData &data) {
        auto it = levelThemeEventListeners.find(event);
        if (it != levelThemeEventListeners.end()) {
            for (const auto &entry: it->second) {
                entry.callback(data);
            }
        }
    }

    void removeListener(const GameEvent event, void *owner) {
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
