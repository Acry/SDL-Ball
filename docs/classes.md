# classes

Basierend auf dem Code in main.cpp gibt es 12 Klassen:

ballManager

powerupManager

bulletsClass
hudClass
object (Basisklasse)
moving_object (erbt von object)
paddle_class (erbt von object)
speedometerClass
tracer

Einige dieser Klassen bilden eine Vererbungshierarchie:
object ist die Basisklasse
moving_object erbt von object
ball, powerupClass erben von moving_object
brick, paddle_class erben von object

Basisklasse
object -> game_object
Vererbungshierarchie
moving_object -> game_object_moving
ball -> game_object_ball
powerupClass -> game_object_powerup
brick -> game_object_brick
paddle_class -> game_object_paddle

```text
                  object
                    |
         +----------+----------+
         |                     |
   moving_object          paddle_class
         |                     |
    +----+----+                |
    |         |                |
   ball    powerupClass      brick
```

Weitere Klassen ohne Vererbung:
- ballManager
- powerupManager
- bulletsClass
- hudClass
- speedometerClass
- tracer