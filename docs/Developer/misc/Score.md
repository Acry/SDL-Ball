# Score

```c++
void brick::hit(EffectManager &fxMan, position poSpawnPos, position poSpawnVel, bool ballHitMe) {
// Basispunkte werden mit Multiplikator und Ballgeschwindigkeit multipliziert
    player.score += score * player.multiply * var.averageBallSpeed;
    }
// ...
{
    brick::breakable() {
    if (type == '3') {
    score = 300; // Zement gibt 300 Punkte
    hitsLeft = 1;
    type = '1';
}
```

Wichtige Erkenntnisse:

Punkte werden mit Geschwindigkeitsbonus berechnet (averageBallSpeed)
Es gibt einen Multiplikator (player.multiply)
Zement-Bricks geben 300 Basispunkte
Die finale Punktzahl ist: `score * multiply * averageBallSpeed`

```c++
int BrickManager::getBaseScore(BrickType type) {
    switch(type) {
    case BrickType::Cement:    return 300;
    case BrickType::Glass:     return 200;
    case BrickType::Invisible: return 250;
    case BrickType::Explosive: return 400;
    default:                   return 100;
    }
}
```

## Future

### Paddle/Wall Collisions

minus score on:

PaddleHitLeftBorder,
PaddleHitRightBorder,

dead on:

PaddleHitLeftBorder,
PaddleHitRightBorder,

### Enemies

- One Boss enemy that goes super slowly to paddle, timed level
- enemies that shoot bullets

- new powerup: shield