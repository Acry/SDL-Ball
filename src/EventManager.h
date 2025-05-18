// EventSystem.h
#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

enum class GameEvent {
    BallHitBorder,
    BallLost,
    BallHitPaddle,
    // weitere Events...
};

struct EventData {
    float posX{0};
    float posY{0};
    int soundID{-1};
    // Weitere Daten nach Bedarf
};

class EventSystem {
private:
    using EventCallback = std::function<void(const EventData&)>;
    std::unordered_map<GameEvent, std::vector<EventCallback>> listeners;

    // Singleton-Pattern
    static EventSystem* instance;
    EventSystem() = default;

public:
    static EventSystem& getInstance() {
        if (!instance) {
            instance = new EventSystem();
        }
        return *instance;
    }

    void addListener(GameEvent event, EventCallback callback) {
        listeners[event].push_back(callback);
    }

    void emit(GameEvent event, const EventData& data) {
        if (listeners.find(event) != listeners.end()) {
            for (const auto& callback : listeners[event]) {
                callback(data);
            }
        }
    }
};

// Globale Zugriffsfunktion
inline EventSystem& eventSystem() {
    return EventSystem::getInstance();
}