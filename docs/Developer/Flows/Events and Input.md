# Events

Der EventDispatcher verteilt InputEvents an spezialisierte InputManager (z.B. MouseManager, KeyboardManager).
Diese InputManager wandeln die Eingaben in GameEvents um und lösen sie aus.
Die eigentliche Spiellogik (z.B. BallManager, PaddleManager) reagiert dann auf die GameEvents, nicht direkt auf
InputEvents.

EventDispatcher → verteilt InputEvents an InputManager
InputManager → erzeugt und löst GameEvents aus
Manager-Klassen (BallManager, PaddleManager, etc.) → reagieren auf GameEvents
GameState-Prüfung kann in InputManager erfolgen, um Events kontextsensitiv zu erzeugen

Beides ist möglich, aber die GameState-Prüfung im EventDispatcher sorgt für eine zentrale Steuerung, welche InputEvents
überhaupt an die InputManager weitergeleitet werden. So werden z.B. im Menü keine Spiel-Inputs verarbeitet.
Die Prüfung im InputManager ist sinnvoll, wenn einzelne Manager unterschiedlich auf GameStates reagieren sollen.
Für eine klare Trennung und Kontrolle empfiehlt sich die Prüfung im EventDispatcher.
Dadurch bleibt die Architektur übersichtlich und die InputManager müssen sich nicht um globale Zustände kümmern.

GameEvents.h
EventDispatcher -> IEventManager

Beispiel: EventDispatcher sendet KeyPressed/KeyReleased, KeyboardManager hört EXKLUSIV auf: KeyPressed/KeyReleased
Andere Komponenten sollten nicht auf KeyPressed/KeyReleased hören, sondern nur z.B. auf GameEvent::KeyboardEvent.

So trennen wir den EventDispatcher von den InputManagern, ich keine direkte Kopplung zum DisplayManager, der sendet ein
Viewport event.

## Future structure

```c++
// Event.h
#include <variant>

enum class EventType {
    Collision,
    Level,
    Theme,
    // ...
};

struct Event {
    EventType type;
    std::variant<CollisionData, LevelData, ThemeData /*, ...*/> data;
};
```

Eigene Header pro Event-Gruppe:
Lege z.B. CollisionEvents.h, LevelEvents.h usw. an.

```c++
struct Event {
enum class Type { Collision, Level, Theme, ... } type;
    // Union oder std::variant für die Eventdaten
};

// GameEvents.h
// CollisionEvents.h
enum class CollisionEvent {
    BallHitLeftBorder,
    BallHitRightBorder,
    BallHitTopBorder,
    BallHitBall,
    BallHitPaddle,
    BallHitBrick,
    // ...
};

// LevelEvents.h
enum class LevelEvent {
    LevelRequested,
    LevelChanged,
    LevelLoaded,
    LevelStarted,
    // ...
};

// ThemeEvents.h
enum class ThemeEvent {
    ThemeRequested,
    ThemeChanged,
    FontThemeRequested,
    FontThemeChanged,
    // ...
};
```
