# GameManager

class GameStateManager {
private:
Ball ball;
Paddle paddle;
std::vector<Brick> bricks;
EffectManager effectManager;
CollisionManager collisionManager;

    void update(float deltaTime) {
        if (currentState != State::RUNNING) return;
        
        ball.update(deltaTime);
        paddle.update(deltaTime);
        collisionManager.checkCollisions(ball, paddle, bricks);
        effectManager.update(deltaTime);
    }

Der SettingsManager sollte hauptsächlich für das Laden, Speichern und Bereitstellen von Einstellungen verantwortlich
sein, nicht für die Spielinitialisierung. Die zentrale GameManager-Klasse sollte das Management aller Subsysteme
übernehmen.

```c++
int main() {
    // Manager initialisieren
    ConfigFileManager configManager;
    SettingsManager settingsManager(configManager);
    ThemeManager themeManager(configManager);
    
    // Game Manager erstellen und initialisieren
    GameManager gameManager(configManager, settingsManager, themeManager);
    gameManager.initializeGameObjects();
    
    // Spielschleife starten
    // ...
}
```

settingsManager.initialize
themeManager.initialize
soundManager.initialize
textureManager.initialize
displayManager.initialize
gameObjectManager.initialize
gameObjectManager.loadLevel?

```c++
class GameSystem {
    public:
    void update(float deltaTime) {
        if (!paused) {
        // Alle Systeme aktualisieren
        for (auto& animation :animationComponents) {
            animation.update(deltaTime);
        }
        // Andere Systeme...
        }
    }
    void setPaused(bool isPaused) {
        this->paused = isPaused;
    }
};
```

Der GameManager übernimmt dann die Koordination:

```c++
class GameManager {
private:
    Player player;
    
public:
    void initGame() {
        auto& settings = SettingsManager::getInstance();
        auto difficulty = settings.getDifficultySettings();
        player.init(difficulty);
        // weitere Initialisierungen...
    }
};
```

Das sollte dann wohl der GameInitManager machen? Also eine Komponente die das Game bootsprapped. Dann vielleicht ein
GameResetManger der mit dem GameUpdateManager spricht. Ich weiß nicht genau.

```c++
class GameInitManager {
private:
    Player& player;
    
public:
    explicit GameInitManager(Player& p) : player(p) {}
    
    void init() {
        auto& settings = SettingsManager::getInstance();
        auto difficulty = settings.getDifficultySettings();
        player.init(difficulty);
        // weitere Initialisierungen...
    }
};

class GameStateManager {
private:
    Player& player;
    
public:
    explicit GameStateManager(Player& p) : player(p) {}
    
    void reset() {
        player.reset();
        // weitere Reset-Logik...
    }
};

class GameManager {
private:
    Player player;
    GameInitManager initManager;
    GameStateManager stateManager;
    
public:
    GameManager() : 
        player(),
        initManager(player),
        stateManager(player) {}
        
    void bootGame() {
        initManager.init();
    }
    
    void resetGame() {
        stateManager.reset();
    }
};
```
