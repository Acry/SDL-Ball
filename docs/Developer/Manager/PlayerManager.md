# PlayerManager

The PlayerManager is responsible for managing the player settings and state in the game. It controls the paddle, lives,
coins, and other player-related attributes. The PlayerManager should not be responsible for initializing the player
directly; instead, it should retrieve the necessary settings from the GameManager, via SettingsManager, and pass them to
the Player class.

lives
coins
powerups
speed

Spawn

Despawn

Textures:
Dein bestehender Ansatz mit den separaten Properties für jede Schicht ist sehr praktisch und leicht verständlich.

ManagedObjects:
Da der PaddleManager nur ein Paddle verwalten soll, könnte der Vector für managedObjects auf ein einzelnes Element
beschränkt werden, aber das Beibehalten für Konsistenz mit anderen Managern ist auch sinnvoll.

Collisions:

## Old code explanation

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
