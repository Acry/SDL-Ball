# SoundManager Todo

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

## future

- [ ] mod support for music and samples (demonstrate in Retro theme)
- https://modarchive.org/ (retro use MOD files)
- [ ] rework positional sound <https://examples.libsdl.org/SDL3/audio/05-planar-data/>