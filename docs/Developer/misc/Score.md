# Score

## The Scoresystem

Das Punktesystem in diesem Spiel basiert auf verschiedenen Faktoren, die die Punktzahl des Spielers beeinflussen. Hier
sind die wichtigsten Aspekte:

When all levels are completed, the game starts from level1 again, but with twice the score.

player.score += score*player.multiply;

Der Multiplikator (player.multiply) wird beim Abschluss aller Levels erhöht. Konkret passiert das im Hauptprogramm nach
dieser Logik:

Wenn der Spieler alle Levels (player.level == var.numlevels) geschafft hat, wird der Multiplikator wie folgt angepasst:

player.multiply += player.multiply * 3;
Das heißt, der aktuelle Multiplikator wird mit 3 multipliziert und zum bisherigen Wert addiert.

Anfangs ist der Multiplikator vermutlich auf 1 gesetzt.

Der Multiplikator beeinflusst die Bonuspunkte am Levelende:

player.score += (bMan.activeBalls * 150) * player.multiply;
Zusammengefasst: Der Multiplikator steigt exponentiell, jedes Mal wenn alle Levels abgeschlossen wurden.

Der Spieler erhält Punkte, wenn ein Powerup aufgesammelt wird. Die Punkte werden mit dem aktuellen Multiplikator (
player.multiply) multipliziert und zum Score addiert.
Minuspunkte werden hier nicht vergeben, sondern nur positive Punkte.

player.score += (bMan.activeBalls*150)*player.multiply;

Diese Zeile vergibt Bonuspunkte am Levelende:
Die Anzahl der aktiven Bälle (bMan.activeBalls) wird mit 150 multipliziert und dann mit dem aktuellen Multiplikator (
player.multiply) verrechnet. Das Ergebnis wird zum Punktestand (player.score) addiert.

So erhält der Spieler zusätzliche Punkte, abhängig von verbleibenden Bällen und Multiplikator.

`player.multiply*3;`
Ja, das bedeutet: Der aktuelle Wert von player.multiply wird mit 3 multipliziert und zum bisherigen Wert addiert.

Beispiel:
Wenn player.multiply vorher 1 war, dann wird daraus 1 + 1*3 = 4.
War er 4, dann wird daraus 4 + 4*3 = 16.

Der Multiplikator wächst also exponentiell:
Neuer Wert = alter Wert + (alter Wert * 3) = alter Wert * 4

Das entspricht einer Vervierfachung des Multiplikators jedes Mal, wenn alle Levels abgeschlossen wurden.

## Old point system

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

## Minuspunkte

Aus `void dropBoard(brick bricks[])`

Das passiert, wenn ein Stein durch das Herunterfallen vom Spielfeld zerstört wird.
Die Zeile `player.score -= bricks[i].score;` zieht den Punktwert eines Steins von der Spielerpunktzahl ab, wenn der
Stein entfernt wird (z.B. beim Herunterfallen des Boards). Das bedeutet, dass der Spieler für diesen Stein Punkte
verliert.

Das ist sinnvoll, wenn das Entfernen des Steins als „Strafe“ gilt, etwa weil der Spieler ihn nicht regulär zerstört hat.

## Wichtige Erkenntnisse

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

## Brick Scores

See: [Bricks](../GameObjects/Brick.md)

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