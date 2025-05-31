// EventManager.h
#pragma once
#include <functional>
#include <vector>
#include <unordered_map>

#include "Brick.h"
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
};

struct EventData {
    float posX{0};
    float posY{0};
    int soundID{-1};
    GameObject *sender{nullptr};
    const GameObject *target{nullptr};
    int points{0};
    // Weitere Daten nach Bedarf
};

class EventManager {
public:
    using EventCallback = std::function<void(const EventData &)>;

    struct CallbackInfo {
        EventCallback callback;
        void *owner = nullptr;

        CallbackInfo(EventCallback cb, void *o = nullptr) : callback(cb), owner(o) {
        }

        void operator()(const EventData &data) const {
            callback(data);
        }
    };

private:
    std::unordered_map<GameEvent, std::vector<CallbackInfo> > listeners;

public:
    EventManager() = default;

    virtual ~EventManager() = default;

    // Kein Kopieren erlauben
    EventManager(const EventManager &) = delete;

    EventManager &operator=(const EventManager &) = delete;

    // Listener-Management
    void addListener(GameEvent event, EventCallback callback, void *owner = nullptr);

    virtual void emit(GameEvent event, const EventData &data);

    // Entfernt alle Listener eines bestimmten Besitzers für ein Event
    void removeListener(GameEvent event, void *owner);

    void removeAllListeners(GameEvent event);

    void clearAllListeners();
};
