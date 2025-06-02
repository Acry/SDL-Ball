// EventManager.h
#pragma once
#include <functional>
#include <vector>
#include <unordered_map>

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
    Uint32 currentlevel{0};
    Uint32 maxLevel{0};
};

using EventCallback = std::function<void(const EventData &)>;
using LevelEventCallback = std::function<void(const LevelEventData &)>;

class EventManager {
private:
    std::unordered_map<GameEvent, std::vector<EventCallback> > eventListeners;
    std::unordered_map<GameEvent, std::vector<LevelEventCallback> > levelEventListeners;

public:
    void addListener(GameEvent event, EventCallback callback, void *owner = nullptr) {
        eventListeners[event].push_back(std::move(callback));
    }

    void addListener(GameEvent event, LevelEventCallback callback, void *owner = nullptr) {
        levelEventListeners[event].push_back(std::move(callback));
    }

    void emit(GameEvent event, const EventData &data) {
        auto it = eventListeners.find(event);
        if (it != eventListeners.end()) {
            for (const auto &callback: it->second) {
                callback(data);
            }
        }
    }

    void emit(GameEvent event, const LevelEventData &data) {
        auto it = levelEventListeners.find(event);
        if (it != levelEventListeners.end()) {
            for (const auto &callback: it->second) {
                callback(data);
            }
        }
    }

    void removeListener(GameEvent event, void *owner) {
        // Da wir owner nicht mehr verwenden, entfernen wir einfach alle Listener für das Event
        eventListeners[event].clear();
        levelEventListeners[event].clear();
    }

    void removeAllListeners(GameEvent event) {
        eventListeners[event].clear();
        levelEventListeners[event].clear();
    }

    void clearAllListeners() {
        eventListeners.clear();
        levelEventListeners.clear();
    }
};
