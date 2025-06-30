# PlayfieldBorder

## Dropping Levels

See: [Level format](../misc/level_format.md)

LevelManager::loadLevel eventManager->emit(GameEvent::LevelLoaded, currentLevelData);

int dropSpeed{0}; // Geschwindigkeit des Drops (wenn isDropping true ist)
down 10000
The above makes the bricks drop down once every 10 seconds.
So it drops one row height every 10 seconds.
