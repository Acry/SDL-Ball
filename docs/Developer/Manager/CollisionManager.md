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
