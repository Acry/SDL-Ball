# Score

## The Scoresystem

Das Punktesystem in diesem Spiel basiert auf verschiedenen Faktoren, die die Punktzahl des Spielers beeinflussen. Hier
sind die wichtigsten Aspekte:

Was denn nun? 2-fach oder 3-fach
When all levels are completed, the game starts from level 1 again, but with twice the score.

player.score += score*player.multiply;

Der Multiplikator (player.multiply) wird beim Abschluss aller Levels erhöht.
Konkret passiert das im Hauptprogramm nach
dieser Logik:

Wenn der Spieler alle Levels (player.level == var.numlevels) geschafft hat, wird der Multiplikator wie folgt angepasst:

old_main.cpp

```c++
player.multiply += player.multiply * 3;

              //If player completed all levels, restart the game with higher multiplier
              if(player.level == var.numlevels)
              {
                player.multiply += player.multiply*3;
                player.level=0;
                announce.write("Finished!",3500,FONT_ANNOUNCE_GOOD);
              }
```

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

Die Punktevergabe erfolgt hauptsächlich beim Treffen von Bricks (Steinen) und beim Abschluss eines Levels.

Hier die wichtigsten Stellen:
Beim Treffen eines Bricks:
Im Code wird beim Kollisions-Handling (coldet) die Methode br.hit(...) aufgerufen.
Die Punkte für einen Brick werden vermutlich in dieser Methode vergeben, typischerweise durch player.score += br.score;.

Bonus beim Levelabschluss:
Wenn ein Level abgeschlossen wird (if(player.level == var.numlevels)), gibt es einen Bonus für verbleibende Bälle:

player.score += (bMan.activeBalls*150)*player.multiply;
Das heißt, für jeden verbleibenden Ball gibt es 150 Punkte, multipliziert mit dem aktuellen Multiplikator.

Beim Entfernen von Bricks durch Drop:
Wenn ein Brick durch das Fallenlassen des Boards entfernt wird (dropBoard), werden Punkte abgezogen:
player.score -= bricks[i].score;

## Minuspunkte

Aus `void dropBoard(brick bricks[])`

Das passiert, wenn ein Stein durch das Herunterfallen vom Spielfeld zerstört wird.
Die Zeile `player.score -= bricks[i].score;` zieht den Punktwert eines Steins von der Spielerpunktzahl ab, wenn der
Stein entfernt wird (z.B. beim Herunterfallen des Boards). Das bedeutet, dass der Spieler für diesen Stein Punkte
verliert.

Das ist sinnvoll, wenn das Entfernen des Steins als „Strafe“ gilt, etwa weil der Spieler ihn nicht regulär zerstört hat.

## Wichtige Erkenntnisse

Basis-Punkte: Für das Zerstören von Bricks werden Punkte vergeben.
Geschwindigkeitsbonus: Die durchschnittliche Ballgeschwindigkeit (averageBallSpeed) beeinflusst die Punkte, je schneller
der Ball, desto höher der Bonus.
Multiplikator: Nach Abschluss aller Level wird der Punktmultiplikator erhöht.
Powerups und Boni: Zusätzliche Punkte gibt es z.B. für mehrere aktive Bälle am Levelende.

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

Beim Abschluss eines Levels:
if(bMan.activeBalls > 1)
{
sprintf(txt, "Bonus: %i", bMan.activeBalls*150);
player.score += (bMan.activeBalls*150)*player.multiply;
announce.write(txt, 2000, FONT_ANNOUNCE_GOOD);
}

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

## Events

- ScoreChanged

Event-Datenstruktur

struct ScoreChangedEventData {
int newScore{0};
int multiplier{1};
};
using ScoreChangedEventCallback = std::function<void(const ScoreChangedEventData&)>;

// EventManager.h
using ScoreChangedListenerEntry = ListenerEntryBase<ScoreChangedEventCallback>;
std::unordered_map<GameEvent, std::vector<ScoreChangedListenerEntry>> scoreChangedListeners;

void addListener(GameEvent event, ScoreChangedEventCallback callback, void* owner) override;
void emit(GameEvent event, const ScoreChangedEventData& data) override;

// EventManager.cpp
void EventManager::addListener(const GameEvent event, ScoreChangedEventCallback callback, void* owner) {
scoreChangedListeners[event].push_back({owner, std::move(callback)});
}
void EventManager::emit(const GameEvent event, const ScoreChangedEventData& data) {
auto it = scoreChangedListeners.find(event);
if (it != scoreChangedListeners.end()) {
for (const auto& entry : it->second) {
entry.callback(data);
}
}
}

BrickManager
eventManager->emit(GameEvent::ScoreChanged, ScoreChangedEventData{newScore, multiplier});

Im PaddleManager nach Score-Update:
eventManager->emit(GameEvent::ScoreChanged, ScoreChangedEventData{score, multiplier});

HUDManager
eventManager->addListener(GameEvent::ScoreChanged,
[this](const ScoreChangedEventData& data) { updateScoreDisplay(data); }, this);

