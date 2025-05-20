#include "EventManager.h"
#include <SDL2/SDL_log.h>

void EventManager::addListener(GameEvent event, EventCallback callback, void* owner) {
    listeners[event].emplace_back(callback, owner);
    SDL_Log("Event-Listener für Event %d hinzugefügt, jetzt %zu Listener",
            static_cast<int>(event), listeners[event].size());
}

void EventManager::emit(GameEvent event, const EventData& data) {
    if (listeners.find(event) != listeners.end()) {
        SDL_Log("Event %d ausgelöst mit Position (%.2f, %.2f)",
                static_cast<int>(event), data.posX, data.posY);

        // Kopiere die Liste der Callbacks, um Probleme zu vermeiden,
        // falls Callbacks während der Verarbeitung hinzugefügt/entfernt werden
        auto callbacksList = listeners[event];
        for (const auto& callbackInfo : callbacksList) {
            callbackInfo.callback(data);
        }
    }
}

void EventManager::removeListener(GameEvent event, void* owner) {
    if (listeners.find(event) != listeners.end()) {
        // Entferne alle Callbacks, die dem angegebenen Besitzer gehören
        auto& callbacksList = listeners[event];
        auto newEnd = std::remove_if(callbacksList.begin(), callbacksList.end(),
                                    [owner](const CallbackInfo& info) {
                                        return info.owner == owner;
                                    });
        size_t removedCount = std::distance(newEnd, callbacksList.end());
        callbacksList.erase(newEnd, callbacksList.end());

        SDL_Log("Event-Listener für Event %d entfernt: %zu Listener gelöscht",
                static_cast<int>(event), removedCount);
    }
}

void EventManager::removeAllListeners(GameEvent event) {
    if (listeners.find(event) != listeners.end()) {
        size_t count = listeners[event].size();
        listeners[event].clear();
        SDL_Log("Alle %zu Event-Listener für Event %d entfernt",
                count, static_cast<int>(event));
    }
}

void EventManager::clearAllListeners() {
    size_t totalCount = 0;
    for (const auto& pair : listeners) {
        totalCount += pair.second.size();
    }
    listeners.clear();
    SDL_Log("Alle Event-Listener gelöscht (gesamt: %zu)", totalCount);
}
