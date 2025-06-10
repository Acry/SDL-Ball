# Current issues

TextureManager_Tests: show current theme, sprite class and move through textures
SoundManager: use enum, no define
Sound test: write current sound
___
Fix all manual Tests
SpriteSheetAnimation: AnimationTiming, no shared timing
gameobjects: Bricks destroyed, active, visible -Fade->or shrink?
___
use InputManager
use DisplayManager as injection for TestHelper with Title?
use testFontTheme in tests

to TestHelper:

```c++
TextManager textManager;
if (!textManager.setTheme("../tests/themes/default")) {
    SDL_Log("Error loading font theme");
    return EXIT_FAILURE;
}
```

___
Ball, Paddle brauchen Manager
BrickTests: InvisibleBricks rot umranden.
PowerupManager.
___
Aus GameObject sollte ich bald den eventManager und die init methode entfernen.
___

Ich habe die Architektur gerade wieder grundlegend verändert, um die Manager-Klassen zu entkoppeln und event getriebenes
Verhalten zu implementieren.

Anhand der Klasse Brick möchte ich die neue Architektur implementieren.
Zu berücksichtigen ist der CollisionManager, das IEventManager, der EventManager, der BrickManager, ICollideable,
GameObject und MovingObject (Powerup, Ball, Paddle).

Die Init-Methode ist in den Actors m.E. nicht nötig. Von Beginn an war im Hinterkopf, dass die GameObjects/Actors
so wenig wie möglich Logik enthalten, weil die Manager-Klassen die Logik übernehmen sollen.

Es war nicht geplant, dass die GameObjects, wie Brick, Paddle, Powerup, Ball oder playfield-border den Collision-Manager
kennen, deshalb habe ich ICollideable implementiert. Und ich möchte nicht, dass die GameObjects den EventManager kennen.
Die Kollisionen sollten die Manager-Klassen übernehmen.

Wir könnten ICollideable auf die grundlegenden geometrischen Eigenschaften reduzieren und die Kollisionslogik in die
Manager verschieben.

GameObject und MovingObject.

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

Brick

```c++
// Brick.h
#pragma once
#include "GameObject.h"
#include "ICollideable.h"

class Brick final : public GameObject, public ICollideable {
    void drawBase() const;

public:
    bool visible{true};
    Color color{};

    // ICollideable Implementation
    float getPosX() const override { return pos_x; }
    float getPosY() const override { return pos_y; }
    float getWidth() const override { return width; }
    float getHeight() const override { return height; }
    bool isActive() const override { return visible; }

    void draw(float deltaTime) override;
};
```

BrickManager

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

Allerdings konnte ich deren ManagerKlassen nicht ohne weiteres, völlig abstrakt entwickeln, das hat meine kognitiven
Fähigkeiten überfordert.

Die volle Funktionalität von Powerups und PowerupManager ist noch nicht implementiert, aber ich bin einen guten Schritt
weiter.

Auch ist nun klar, welcher teil des codes den Ablauf steuert. Der CodeManager instanziiert alle anderen Manager und
steuert den Ablauf. Er spricht direkt mit dem SettingsManager, dem Theme-Manager und dem configfileManager.
Der EventManager ist die zentrale Kommunikationsschnittstelle.

Der SceneManager ist noch nicht implementiert, er steuert die Szenen, also die verschiedenen Spielzustände (z.B. Titel,
Autoplay, Credits Hauptmenü, Spiel, Pause, Game Over).

Interagiert der Nutzer mit dem Programm, feuert der EventManager und der gameManager übernimmt, solange bis das Spiel
beendet ist oder angehalten wird.
Pause, GameOver, Menu, sind Szenen, die der SceneManager steuert.

GameManager, CollisionManager, EventManager, AudioManager, EffectManager,
BackgroundManager, PowerupManager, BallManager, PlayerManager, LevelManager
HudManager: clock, leben, score, level

SaveGameManager, HighScoreManager, MenuManager

## SpritesheetAnimation - kumuliertes Timing

[AnimationTiming](misc/Animation-Timing.md)

## CollisionManager <->  EventManager <-> GameManager

In einer guten Spielarchitektur sollte die Kollisionsbehandlung auf folgende Weise aufgeteilt werden:

CollisionManager:

- Erkennt Kollisionen zwischen Objekten (checkCollision)
- Berechnet Kollisionsrichtungen und Überlappungen
- Ruft die onCollision-Methoden der beteiligten Objekte auf

Die Objekte selbst (über ICollideable):
-Implementieren ihre spezifische Reaktion auf Kollisionen
-Beispiel: Ein Brick deaktiviert sich beim Treffer

EventManager:
Verbreitet Kollisionsereignisse an interessierte Komponenten
Beispiel: Punkte vergeben, Soundeffekte abspielen

GameManager:
Reagiert auf höherer Ebene (Level abgeschlossen wenn alle Bricks zerstört)

## Extracting Powerup and PowerupManager from Main

Ich denke, ich nähere mich einer sauberen Lösung, die das Singleton-Prinzip vermeidet und die Komponenten entkoppelt.

Audiowiedergabe bei Kollisionen:
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

Diese Architektur erlaubt es jedem System (Audio, Effekte, Logik), auf Events zu reagieren, ohne die andere Komponenten
direkt kennen zu müssen. Der EventManager fungiert als zentrale Vermittlungsstelle, wodurch die Komponenten lose
gekoppelt bleiben.

## GameActorManagers <-> CollisionManager

Die Frage ist, wer mit dem CollisionManager kommuniziert:

BallManager/PowerupManager CollisionManager
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

Danach kann die Border Kollisionsprüfung aus dem Paddle und dem Ball entfernt werden.

Grundsätzlich haben wir 3 Typen von Einstellungen:

- compileTimeSettings, config.h -
- loadedSettings - settings.cfg, SaveGame, Highscore
- current - runtime settings

CollisionManager: Prüft Kollisionen
PowerupManager: Verwaltet Powerups (Spawning, Update, Rendering)
EffectManager: Erzeugt visuelle Effekte
EventManager: Verteilt Events im System
GameManager: Steuert die Spiellogik
AudioManager:
ConfigFileManager: verwaltet die Konfigurationsdateien
SettingsManager: verwaltet die Einstellungen

EffectManager und AudioManager sollten angeglichen werden.
Man sieht im EffectManager schön, wie die Events genutzt werden.
Ich weiß nur nicht, ob die `handleEvents`-Methode im EventManager so konkret benannt werden sollte.

handleBallPaddleCollision, vielleicht wäre etwas Abstraktion sinnvoll.

GameManager als Koordinator:

- Hat die Hauptupdate-Schleife
- Ruft die Update-Methoden der einzelnen Manager auf
- Delegiert Kollisionsprüfungen an den CollisionManager

CollisionManager als Kollisionsservice:

CollisionManager als Kollisionsservice:

- Bietet verschiedene Kollisionsprüfungsfunktionen
- Kennt nur die Kollisionslogik, nicht die Spiellogik
- Emittiert bei erkannten Kollisionen Events über den EventManager

EventManager als Nachrichtensystem:

Ermöglicht die lose Kopplung zwischen Kollisionserkennung und -behandlung
Manager können Events abonnieren und auf diese reagieren

GameManager CollisionManager EventManager
| | |
|--update()                    | |
| | | |
| |--checkAllCollisions()---->| |
| |--checkCollision()          |
| | | |
| | |--emitEvent()----------->|--notify()-->|
| | | |
| |<---return-------------------| |
|<---return-------------------| | |
| |
| |
|<--------------------------------------------------------------handleEvent()

Mit diesem Ansatz:

1. Der GameManager weiß WER kollidieren kann
2. Der CollisionManager weiß WIE Kollisionen erkannt werden
3. Der EventManager verbindet die Kollisionserkennung mit den Reaktionen

Damit brauchst du keine Border-Kollisionsprüfung mehr in Ball und Paddle, und die Manager bleiben fokussiert auf ihre
Kernaufgaben.

Für die Methode im EventManager würde ich tatsächlich einen allgemeineren Namen als handleBallPaddleCollision empfehlen.
Vielleicht subscribe(EventType, Callback) und emit(EventType, EventData), sodass du Event-spezifische Logik in den
jeweiligen Managern behältst und der EventManager nur als Nachrichtensystem fungiert.

Diese Architektur würde Clean Architecture-Prinzipien folgen und gleichzeitig eine funktionale Programmierung mit klaren
Datenflüssen ermöglichen, ohne in OOP-Exzesse zu verfallen.
