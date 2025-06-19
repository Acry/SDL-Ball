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

## old code

Der Player wird hauptsächlich durch die globale Struktur player_struct player repräsentiert, die verschiedene
Spielerattribute wie Score, Level, Lives und Powerups enthält.

In initNewGame() werden die Grundwerte des Spielers zurückgesetzt
In resetPlayerPowerups() werden die Powerups zurückgesetzt
An verschiedenen Stellen im Hauptcode werden Player-Attribute aktualisiert (z.B. Score, Lives)
Das Paddle wird durch die paddle_class repräsentiert, die direkt in der main-Funktion instanziiert wird:
paddle_class paddle;
paddle.tex = texPaddleBase;
paddle.layerTex = texPaddleLayers;

Die Steuerung des Paddles erfolgt über die controllerClass:
controllerClass control(&paddle, &bullet, &bMan);

controllerClass
Diese Klasse verwaltet die Eingabe (Tastatur, Maus, Joystick) und bewegt das Paddle entsprechend. Kollisionsabfragen
zwischen Ball und Paddle werden in der padcoldet()-Funktion durchgeführt.

Die Player- und Paddle-Verwaltung ist mehr in der Hauptschleife verstreut und verwendet globale Variablen und direkte
Referenzen.
