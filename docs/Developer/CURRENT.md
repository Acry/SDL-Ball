# Extracting Powerup and PowerupManager from Main

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

## old

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
