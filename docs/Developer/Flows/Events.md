# Events

EventDispatcher
// GameEvents.h
GameEvents
IEventManager
EventManager

So trennen wir den EventDispatcher von den InputManagern, ich keine direkte Kopplung zum DisplayManager, der sendet ein
Viewport event.

## future structure

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
