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

Der direkte Zugriff auf das Paddle über den EventManager (paddle = eventManager->getPaddle()) ist nicht optimal. Das
erzeugt unnötige Abhängigkeiten.

