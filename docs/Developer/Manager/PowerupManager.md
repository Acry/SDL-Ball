# PowerupManager

Powerups gives you "coins" (you never now exactly how many coins you have),
and you can spend them on... Powerups :D

GameManager: Steuert die Spiellogik
EventManager: Verwaltet Events
CollisionManager: Kümmert sich um Kollisionsabfragen
EffectManager: Steuert visuelle Effekte
PowerupManager: Verwaltet Powerups
BallManager: Verwaltet Bälle

Der PowerupManager hat die Verantwortung für:

Spawnen von Powerups
Aktualisieren ihrer Position
Zeichnen der Powerups

## Integration in das bestehende System

Basierend auf dem Code-Kontext schlage ich folgende Integration des PowerUpManagers vor:

```cpp
// PowerUpManager.h
class PowerUpManager {
    std::vector<PowerUp> activePowerUps;
    EventManager* eventManager;
    TextureManager* textureManager;
    
    void onBrickDestroyed(const EventData& data);

public:
    PowerUpManager(EventManager* evtMgr, TextureManager* texMgr);
    ~PowerUpManager();

    void setupPowerUps(PowerupData data);
    void update(float deltaTime);
    void draw(float deltaTime);
    void clear();
};
```

```cpp
// PowerUpManager.cpp
PowerUpManager::PowerUpManager(EventManager* evtMgr, TextureManager* texMgr) 
    : eventManager(evtMgr), textureManager(texMgr) {
    
    // Auf Brick-Zerstörung hören für PowerUp-Aktivierung
    eventManager->addListener(GameEvent::BrickDestroyed, 
        [this](const EventData& data) { onBrickDestroyed(data); }, 
        this);
}

void PowerUpManager::setupPowerUps(PowerupData data) {
    clear();
    // PowerUps aus LevelManager-Daten initialisieren
    for (const auto& info : data.powerups) {
        if (info.type != '0') {
            // PowerUp Position und Typ speichern
            // Wird erst aktiv wenn der zugehörige Brick zerstört wird
        }
    }
}

void PowerUpManager::onBrickDestroyed(const EventData& data) {
    // Prüfen ob zerstörter Brick ein PowerUp hatte
    // Wenn ja: PowerUp aktivieren und fallen lassen
}
```

Integrationsschritte:

1. PowerUpManager als Member zur Game-Klasse hinzufügen
2. Beim Level-Laden:

```cpp
void Game::loadLevel(size_t level) {
    auto brickData = levelManager.getBrickDataForLevel(level);
    auto powerupData = levelManager.getPowerupDataForLevel(level);
    
    brickManager.setupBricks(brickData);
    powerUpManager.setupPowerUps(powerupData); 
}
```

3. In der Game-Loop:

```cpp
void Game::update(float deltaTime) {
    brickManager.update(deltaTime);
    powerUpManager.update(deltaTime);
    // ...
}

void Game::draw(float deltaTime) {
    brickManager.draw(deltaTime);
    powerUpManager.draw(deltaTime);
    // ...
}
```

Der PowerUpManager:

- Erhält PowerUp-Daten vom LevelManager
- Reagiert auf Brick-Zerstörung Events
- Verwaltet aktive PowerUps (Falling, Collection, Effects)
- Verwendet TextureManager für PowerUp-Grafiken