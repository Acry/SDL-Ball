# CollisionManager

ObjectManager - CollisionManager
| |
| checkCollision(...)? |
|------------------------------>|
| |
| Ergebnis |
|<------------------------------|
| |
| emitEvent bei Kollision |
|------------------------------>|

GameManager CollisionManager
| |
| checkCollision(...)? |
|---------------------------------->|
| |
| Ergebnis |
|<----------------------------------|
| |
| emitEvent bei Kollision |
|---------------------------------->|

class CollisionManager {
public:
void checkCollisions(Ball& ball, Paddle& paddle, const std::vector<Brick>& bricks);
};

Der CollisionManager könnte die Kollisionen erkennen und den EffectManager für die visuellen/akustischen Effekte bei
Kollisionen benachrichtigen:

1. **Interface für Kollidierbare Objekte erstellen**:
    - `ICollidable`-Interface definieren
    - `brick` und `Paddle` so anpassen, dass sie dieses Interface implementieren

```c++
#pragma once
#include "EventManager.h"
#include "Ball.h"
#include "Paddle.h"

class CollisionManager {
    EventManager* eventManager;

public:
    explicit CollisionManager(EventManager* eventMgr) : eventManager(eventMgr) {}

    bool checkBallPaddleCollision(Ball& ball, Paddle& paddle, CollisionPoint& cp) {
        // Kollisionslogik
        if (/* Kollision erkannt */) {
            // Event auslösen
            EventData data;
            data.posX = ball.pos_x;
            data.posY = ball.pos_y;
            data.soundID = SND_BALL_HIT_PADDLE;
            eventManager->emit(GameEvent::BallHitPaddle, data);

            // Abprallwinkel berechnen
            // ...
            return true;
        }
        return false;
    }
};
```

```c++
// Im Hauptprogramm/GameLoop:
CollisionPoint colPoint;
if (CollisionManager::checkBallPaddleCollision(ball, paddle, colPoint)) {
    // Reaktion auf Kollision
    // z.B. Sound abspielen, Punkte vergeben, etc.
}
```

## collision side

enum class CollisionSide {
None,
Top,
Bottom,
Left,
Right
};

## Kollisionsbehandlung basierend auf der Form und Masse der Objekte ist sehr sinnvoll.

bislang wird ein gameobject nur durch den namen indentifiziert.

Aber bälle sind rund

wände sind flach

das paddle ist horizontal convex

also könnte man collisionscode aufgrund der form und oder der masse eine objects implementieren.

Erweiterung der ICollideable-Schnittstelle:

[[nodiscard]] virtual CollisionShape getCollisionShape() const = 0;
[[nodiscard]] virtual float getMass() const = 0;

Spezialisierte Kollisionsbehandlungen:

Kreis-zu-Rechteck (Ball zu Block)
Kreis-zu-Konvex (Ball zu Paddle)
Kreis-zu-Linie (Ball zu Wand)
Kreis-zu-Kreis (Ball zu Ball)

enum class CollisionShape {
Circle,
Rectangle,
Convex,
Line,
CompoundShape // Für komplexere Objekte aus mehreren G