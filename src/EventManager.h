// EventManager.h
#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <algorithm>

enum class GameEvent {
    BallHitBorder,
    BallLost,
    BallHitPaddle,
    BallHitBrick,     // Neues Event für Kollision mit Brick
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
public:
    using EventCallback = std::function<void(const EventData&)>;

    struct CallbackInfo {
        EventCallback callback;
        void* owner = nullptr;

        CallbackInfo(EventCallback cb, void* o = nullptr) : callback(cb), owner(o) {}

        void operator()(const EventData& data) const {
            callback(data);
        }
    };

private:
    std::unordered_map<GameEvent, std::vector<CallbackInfo>> listeners;

public:
    EventManager() = default;
    virtual ~EventManager() = default;

    // Kein Kopieren erlauben
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    // Listener-Management
    void addListener(GameEvent event, EventCallback callback, void* owner = nullptr);

    virtual void emit(GameEvent event, const EventData& data);

    // Entfernt alle Listener eines bestimmten Besitzers für ein Event
    void removeListener(GameEvent event, void* owner);

    void removeAllListeners(GameEvent event);
    void clearAllListeners();
};