# CollisionManager

```c++
// Im Hauptprogramm/GameLoop:
CollisionPoint colPoint;
if (CollisionManager::checkBallPaddleCollision(ball, paddle, colPoint)) {
    // Reaktion auf Kollision
    // z.B. Sound abspielen, Punkte vergeben, etc.
}
```
