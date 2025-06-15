# Current issues

Top: Fix all current manual Tests

## BrickManager tests

try out test context approach
first fix texture usage

## Playfield borders

I deleted the playfieldborder-test, that was not clever, it was there for a reason.
I was working towards dynamic borders, e.g. dropping levels.
Also, a theme should define the playfield border, so it can be different for each theme.
Arkanoid needs a smaller playfield.
We can add theme meta data, like enemies, true/false and different difficulties.
Life, Ballspeed, Ball size, Paddle size, etc. are all theme related.

## Texture Manager

TextureManager event, emit Sprites cached?

## SoundManager

use enum, no define
Sound test: write current sound

## BrickTests

Bricks destroyed, active, visible -Fade->or shrink?
Render all Bricks
InvisibleBricks rot umranden.
Click a Brick to hit

## Missing Mannagers

Ball,
Paddle,
SceneManager:
Autoplay, Credits Hauptmenü, Spiel, Pause, Game Over)
SaveGameManager,
HighScoreManager,
MenuManager
HudManager: clock, leben, score, level

## GameObjects, Managers, collide

Die Init-Methode ist in den Actors m.E. nicht nötig. Von Beginn an war im Hinterkopf, dass die GameObjects/Actors
so wenig wie möglich Logik enthalten, weil die Manager-Klassen die Logik übernehmen sollen.

Wir könnten ICollideable auf die grundlegenden geometrischen Eigenschaften reduzieren und die Kollisionslogik in die
Manager verschieben.

```c++
// ICollideable.h
#pragma once

class ICollideable {
public:
    virtual ~ICollideable() = default;

    // Nur grundlegende Geometrie
    virtual float getPosX() const = 0;
    virtual float getPosY() const = 0;
    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;
    virtual bool isActive() const = 0;
};
```

## BrickManager collision

[ ] - use SpritesheetManager to register animations, see SpriteSheetManager_Tests
[ ] - use EventManager to emit events on collision
[ ] - for dropping level, brick paddle collsion need to be checked

```c++
// BrickManager.h
class BrickManager {
    // ... bestehender Code ...

    void handleCollision(size_t brickIndex, const ICollideable* other) {
        if (auto* ball = dynamic_cast<const Ball*>(other)) {
            EventData data;
            data.target = &bricks[brickIndex];
            onBrickHit(data);
        }
    }
};
```

## Code-Architektur - CodeManager

Auch ist nun klar, welcher teil des codes den Ablauf steuert. Der CodeManager instanziiert alle anderen Manager und
steuert den Ablauf. Er spricht direkt mit dem SettingsManager, dem Theme-Manager und dem configfileManager. Der
EventManager ist die zentrale Kommunikationsschnittstelle.

Der SceneManager ist noch nicht implementiert, er steuert die Szenen, also die verschiedenen Spielzustände (z.B. Titel,

Interagiert der Nutzer mit dem Programm, feuert der EventManager und der gameManager übernimmt, solange bis das Spiel
beendet ist oder angehalten wird.
Pause, GameOver, Menu, sind Szenen, die der SceneManager steuert.

GameManager, CollisionManager, EventManager, AudioManager, EffectManager,
BackgroundManager, PowerupManager, BallManager, PlayerManager, LevelManager
HudManager: clock, leben, score, level

## GameManager

GameManager:
Reagiert auf höherer Ebene (z.B. Level abgeschlossen wenn alle Bricks zerstört)

## Audiowiedergabe bei Kollisionen

Der EventManager ist hier der Schlüssel. Wenn ein Ball die obere Grenze trifft, läuft folgende Kette ab:

1. `CollisionManager::checkCollisionWithBorder` erkennt die Kollision
2. `PlayfieldBorder::onCollision` wird aufgerufen und emittiert ein Event (`GameEvent::BallHitTopBorder`)
3. Im `GameManager::setupEventHandlers()` werden Event-Listener registriert, die auf solche Events reagieren

Der Audiomanager sollte dort als Listener registriert sein:

```cpp
// In GameManager::setupEventHandlers
eventManager.addListener(GameEvent::BallHitTopBorder, 
    [this](const EventData& data) {
        // Sound abspielen
        soundManager.playSound("borderHit");
        
        // Weitere Aktionen...
    },
    this
);
```

## GameActorManagers <-> CollisionManager

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
