# ManagerClasses

Grundsätzlich haben wir 3 Typen von Settings:
- default aus config.h - compileTime settings
- loaded - gespeicherte Settings des Spielers
- current - und die runtime settings

**Singletons provide a global access mechanism to an object.**
**Singletons are a global variable dressed up in some awkward syntax.**

Manager Typen:

[SettingsManager](Manager/SettingsManager.md) spricht mit dem ConfigFileManager, hält defaults und current settings

[GameManager](Manager/GameManager.md) - GameState -> BallManager, Paddle, Bricks?

MenuManager spricht mit dem SettingsManager, GameManager, ControllerManager

To make the game building, let the settingsManager init the player.
The current Blocker is gorilla-banana style, I really don't want to construct the player like this:

`const auto& settings = SettingsManager::getInstance(configFileManager);`

SettingsManager::getInstance().init(configFileManager);

const auto& settings = SettingsManager::getInstance();

Wobei ich noch nicht weiß, ob ich will, dass der Player mit dem SettingsManager spricht.
Basierend auf dem Clean Architecture Prinzip sollte der Player tatsächlich nicht direkt mit dem SettingsManager kommunizieren. Stattdessen sollten die Einstellungen während der Initialisierung injiziert werden:

```c++


class Player {
private:
    difficultyStruct currentDifficulty;

public:
    Player() = default;
    
    void init(const difficultyStruct& difficulty) {
        currentDifficulty = difficulty;
        lives = difficulty.life;
        coins = difficulty.coins;
        // weitere Initialisierungen...
    }
    // ... rest der Klasse
};
```

NEED TO TEST THIS with config and settings. -> use ConfigFileManager_Tests.cpp
**auto& settings = SettingsManager::getInstance();**
**auto difficulty = settings.getDifficultySettings();**

Player player;
player.init(difficulty);

Die sauberste Lösung wäre, dass der GameManager die Spieler-Einstellungen vom SettingsManager holt und diese dann an den Player weiterreicht. Dabei sollte der Player nur die Daten kennen, die er wirklich braucht:

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

Das sollte dann wohl der GameInitManager machen? Also eine Komponente die das Game bootsprapped. Dann vielleicht ein GameResetManger der mit dem GameUpdateManager spricht. Ich weiß nicht genau.

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

Die Spielobjekte sollten möglichst "dumm" sein und nur ihre grundlegenden Eigenschaften und Verhaltensweisen kennen.

class CollisionManager {
public:
    void checkCollisions(Ball& ball, Paddle& paddle, const std::vector<Brick>& bricks);
};

Der CollisionManager könnte die Kollisionen erkennen und den EffectManager für die visuellen/akustischen Effekte bei Kollisionen benachrichtigen:

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

UI/HUD/MENU

Die klassischen states, mh. Ich sehe die eher als hierrachische spielszenen statt: `enum class State { RUNNING, PAUSED, GAME_OVER }; State currentState`;

SceneManager: switch - PlayScene, GameOverScene, TitleScene, (SettingsScene)
        changeScene(std::move(pauseScene));
        menuManager.showPauseMenu();

Pretty much, a top down approach.

## Current Managers

[BackgroundManager](Manager/BackgroundManager.md) - handles the background, loads the background file and applies it to the game
[BallManager](Manager/BallManager.md) - handles the balls, loads the ball file and applies it to the game
[ConfigFileManager](Manager/ConfigFileManager.md) - handles config pathes, evaluates and loads the config file.
[ControllerManager](Manager/ControllerManager.md) - handles the controller, and settings
[EffectManager](Manager/EffectManager.md) - handles the effects, loads the effect file and applies it to the game
[DisplayManager](Manager/DisplayManager.md) - handles the display, loads the display file and applies it to the game
[GameManager](Manager/GameManager.md) - handles the game state, loads the game and manages the game loop
[HighscoreManager](Manager/HighscoreManager.md) - handles the highscores, loads the highscores and saves them to the file.
[LevelManager](Manager/LevelManager.md) - at start loads the LevelOffsets, and gives the new level after the player has finished the level.
[SaveGameManager](Manager/SaveGameManager.md) - handles the savegame, loads the savegame and saves it to the file
[SettingsManager](Manager/SettingsManager.md) - handles the settings, loads the settings and saves them to the file
[SoundManager](Manager/SoundManager.md) - Loading/Cleaning and playing sounds
[TextureManager](Manager/TextureManager.md)
[ThemeManager](Manager/ThemeManager.md) - handles the themes, loads the theme file and applies it to the game
MenuManager - handles the menu, loads the menu file and applies it to the game

## Scenes

TitleScene
InstructionsScene
PlayScene
PauseScene
GameOverScene
LevelDoneScene
HighscoreScene
GameScene
GameOverScene
PauseScene
SettingsScene

See: [Structure](Structure.md)

## OOP

OOP is and has always been broken by design.
Classes are factories. which are barely needed.
OOP is an end in itself, totally abstracted to implement the blueprint of the blueprint.
I dont want that, oh, make it singleton, make it a class, make it a factory, make it a template, make it a lambda, make it a function, make it a macro.
I want to write code, not classes or templates.
Yeah, and encapsulation protects a class of a coder. =)
Inheritenace, Polymorphism - like dependencies: gorilla - banana.

"Object-oriented programming is an exceptionally bad idea which could only have originated in California."
-  Edsger W. Dijkstra

That said, happy hacking.


P.S.

Functional programming is not about lambdas, it is all about pure functions.

So broadly promote functional style:

- Only use function arguments, do not use global state.
- Keep 
- Minimise side effects.

This can be achieved in plain c, no need for magic.
<https://research.utwente.nl/files/5128727/book.pdf>

Use the constructor for dependency injection, but don't overdo it. 
