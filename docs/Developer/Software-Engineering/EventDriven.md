# Event-Driven Architektur

Basierend auf deinem Code erkläre ich die Event-Driven Architektur:

**Event-Driven Programmierung** in deinem Projekt:

1. **Event Interface**

```cpp
class IEventManager {
    virtual void emit(GameEvent event, const EventData &data) = 0;
    virtual void addListener(GameEvent event, EventCallback callback, void *owner) = 0;
    virtual void removeListener(GameEvent event, void *owner) = 0;
};
```

2. **Event-Sender (Publisher)**

```cpp
// In BrickManager
void onBrickHit(const EventData &data) {
    eventManager->emit(GameEvent::BrickDestroyed, data);
}
```

3. **Event-Empfänger (Subscriber)**

```cpp
// Beispiel für Registrierung
eventManager->addListener(GameEvent::BrickDestroyed, 
    [this](const EventData& data) { /* Handler */ }, 
    this);
```

Vorteile dieser Architektur:

- Lose Kopplung zwischen Komponenten
- Bessere Skalierbarkeit
- Asynchrone Verarbeitung möglich
- Einfache Erweiterbarkeit

In deinem Code sieht man dies besonders gut bei der Kollisionserkennung und dem Brick-Management.
