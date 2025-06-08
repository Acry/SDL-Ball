# PlayerManager

The PlayerManager is responsible for managing the player settings and state in the game. It controls the paddle, lives,
coins, and other player-related attributes. The PlayerManager should not be responsible for initializing the player
directly; instead, it should retrieve the necessary settings from the SettingsManager and pass them to the Player class.

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
