// EventManager.h
#pragma once
#include <functional>
#include <vector>
#include <unordered_map>

enum class GameEvent {
    BallHitBorder,
    BallLost,
    BallHitPaddle,
    BrickDestroyed,
    PowerUpCollected,
    LevelCompleted,
    GameOver
    // Weitere Events können nach Bedarf hinzugefügt werden
};

struct EventData {
    float posX{0};
    float posY{0};
    int soundID{-1};
    void* sender{nullptr};
    void* target{nullptr};
    int points{0};
    // Weitere Daten nach Bedarf
};

class EventManager {
    using EventCallback = std::function<void(const EventData&)>;
    std::unordered_map<GameEvent, std::vector<EventCallback>> listeners;

public:
    EventManager() = default;
    virtual ~EventManager() = default;

    // Kein Kopieren erlauben
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    void addListener(GameEvent event, EventCallback callback) {
        listeners[event].push_back(callback);
    }

    virtual void emit(GameEvent event, const EventData& data) {
        if (listeners.find(event) != listeners.end()) {
            for (const auto& callback : listeners[event]) {
                callback(data);
            }
        }
    }

    void removeAllListeners(GameEvent event) {
        if (listeners.find(event) != listeners.end()) {
            listeners[event].clear();
        }
    }

    void clearAllListeners() {
        listeners.clear();
    }
};
