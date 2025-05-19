# Publish-Subscribe-Pattern und IoC in der Spieleentwicklung

Das Publish-Subscribe-Pattern und Inversion of Control (IoC) bieten hervorragende Möglichkeiten, die Architektur eines komplexen Spiels sauberer zu gestalten. Besonders bei der Interaktion zwischen EventManager, EffectManager und GameManager können diese Patterns große Vorteile bringen.

## Publish-Subscribe im Spielkontext

Im Publish-Subscribe-Pattern:

- **Publisher** (Sender) veröffentlichen Events, ohne zu wissen, wer diese empfängt
- **Subscriber** (Empfänger) abonnieren bestimmte Events und reagieren darauf
- Der **EventManager** dient als zentraler Broker zwischen diesen Parteien

### Vorteile für unsere Spiel-Manager

1. **EventManager als zentraler Broker**
    - Entkoppelt Sender und Empfänger vollständig
    - Erlaubt dynamisches Hinzufügen/Entfernen von Listenern zur Laufzeit
    - Ermöglicht Broadcasting an mehrere Interessenten

2. **GameManager als Publisher**
    - Veröffentlicht Spielzustandsänderungen: `LEVEL_STARTED`, `GAME_OVER`, `PAUSE_STATE_CHANGED`
    - Triggert Spielevents wie `BRICK_DESTROYED`, `BALL_LOST`, `POWERUP_COLLECTED`
    - Muss nicht wissen, wer diese Events konsumiert (UI, Effekte, Sound, Achievements)

3. **EffectManager als Subscriber**
    - Reagiert auf relevante Spielevents: `EXPLOSION`, `BALL_COLLISION`, `POWERUP_SPAWN`
    - Kann Effekte ohne direkte Abhängigkeit zur Spiellogik erzeugen
    - Erlaubt visuelle Konsistenz bei ähnlichen Events

4. **CollisionManager als Vermittler**
   - Erkennt Kollisionen zwischen Spielobjekten
   - Löst Events aus, die von anderen Managern verarbeitet werden
   - Verhindert direkte Abhängigkeiten zwischen GameManager und EffectManager

5. **SoundManager als weiterer Subscriber**
   - Reagiert auf Events wie `EXPLOSION`, `POWERUP_COLLECTED`
   - Kann Sounds abspielen, ohne die Quelle der Events zu kennen
   - Ermöglicht einfache Erweiterung um neue Soundeffekte

6. **UIManager als Subscriber**
   - Aktualisiert die Benutzeroberfläche basierend auf Spielereignissen
   - Reagiert auf Events wie `SCORE_UPDATED`, `LIVES_CHANGED`
   - Ermöglicht eine klare Trennung zwischen Spiellogik und UI-Logik

7. **MenuManager als Subscriber**
   - Reagiert auf Events wie `MENU_OPENED`, `MENU_CLOSED`
   - Ermöglicht eine klare Trennung zwischen Spiel- und Menülogik
   - Kann Menüs dynamisch basierend auf Spielereignissen aktualisieren

## Inversion of Control für mehr Flexibilität

IoC kehrt die Kontrollrichtung um: Statt dass Komponenten aktiv andere steuern, werden sie vom Framework aufgerufen.

### Anwendung in unserem Spieldesign

1. **Dependency Injection für Manager**
    - Manager werden nicht mehr als Singletons implementiert
    - Stattdessen werden Abhängigkeiten explizit übergeben
    - Beispiel: `GameManager(EffectManager&, EventManager&, CollisionManager&)`

2. **Konfigurationsbasierte Entkopplung**
    - Effekte werden über Konfigurationsdateien definiert, nicht hart kodiert
    - Events können zur Laufzeit registriert und modifiziert werden
    - Theme-Abhängigkeiten (wie Effekttypen) werden deklarativ ausgedrückt

3. **Callback-basierte Interaktionen**
    - Manager registrieren Callbacks beim EventManager
    - Spielobjekte lösen Events aus, statt direkt Manager aufzurufen
    - Reduziert zyklische Abhängigkeiten erheblich

## Konkrete Beispiele für Manager-Beziehungen

### EventManager als Herzstück

Der EventManager sollte:
- Eine einfache Registrierung von Listenern ermöglichen
- Priorisierung von Event-Handlern unterstützen
- Asynchrones Event-Handling für zeitkritische Operationen bieten

```cpp
// Beispiel für EventManager-Interface (ohne Implementierung)
class EventManager {
public:
    // Registrierung von Listenern
    virtual void registerListener(IEventListener* listener, EventType eventType, int priority = 0) = 0;
    virtual void unregisterListener(IEventListener* listener, EventType eventType = EVENT_ALL) = 0;
    
    // Event-Verarbeitung
    virtual void triggerEvent(const GameEvent& event) = 0;
    virtual void queueEvent(const GameEvent& event) = 0;
    virtual void processQueuedEvents() = 0;
};
```

### EffectManager als visueller Dienst

Der EffectManager sollte:
- Eine ressourcen-effiziente Verwaltung von Partikeleffekten bieten
- Auf Events reagieren, ohne die genaue Spiellogik zu kennen
- Parameter für Effekte über die `set()`-Methoden konfigurieren

```cpp
// EffectManager-Integration mit dem EventManager
class EffectManager : public IEventListener {
public:
    // Event-Handling-Implementation
    void onEvent(const GameEvent& event) override {
        switch (event.type) {
            case EVENT_BRICK_DESTROYED:
                handleBrickDestruction(event.data.brickData);
                break;
            case EVENT_POWERUP_COLLECTED:
                handlePowerupCollection(event.data.powerupData);
                break;
            case EVENT_BALL_COLLISION:
                handleBallCollision(event.data.collisionData);
                break;
            // weitere Events...
        }
    }
    
    // Bestehende EffectManager-Methoden (set, spawn, usw.)
    // ...
};
```

### GameManager als Koordinator

Der GameManager sollte:
- Den Spielzustand verwalten, ohne Rendering-Details zu kennen
- Events für signifikante Änderungen auslösen
- Spielobjekte über Services wie den CollisionManager koordinieren

```cpp
// Beispiel für GameManager-Integration
class GameManager {
private:
    EventManager& eventMgr;
    // weitere Manager-Referenzen...

public:
    GameManager(EventManager& eventManager, /* weitere Manager... */)
        : eventMgr(eventManager) {
        // Initialisierung...
    }
    
    void startLevel(int levelNumber) {
        // Level-Logik...
        
        // Event auslösen statt direkte Aufrufe
        GameEvent event;
        event.type = EVENT_LEVEL_STARTED;
        event.data.levelData.number = levelNumber;
        eventMgr.triggerEvent(event);
    }
    
    // weitere Spielverwaltungsfunktionen...
};
```

## Verbesserte Architektur

Durch Anwendung dieser Patterns entsteht eine verbesserte Architektur:

1. **Reduzierte Abhängigkeiten**
    - Manager kennen nur die Interfaces anderer Manager, nicht deren Implementierungen
    - Zirkuläre Abhängigkeiten werden vermieden

2. **Bessere Testbarkeit**
    - Manager können mit Mock-Objekten getestet werden
    - Event-basierte Kommunikation kann leicht simuliert werden

3. **Einfachere Erweiterbarkeit**
    - Neue Effekte oder Features können ohne Änderungen bestehender Komponenten hinzugefügt werden
    - Komponentenbasiertes Design statt monolithischer Klassen

4. **Klare Verantwortlichkeiten**
    - GameManager: Spiellogik und -zustand
    - EventManager: Kommunikation zwischen Komponenten
    - EffectManager: Visuelle Effekte und Partikelsysteme
    - CollisionManager: Kollisionserkennung und -reaktion

Diese saubere, ereignisgesteuerte Architektur macht den Code wartbarer und schafft eine gute Grundlage für zukünftige Erweiterungen des Spiels.
