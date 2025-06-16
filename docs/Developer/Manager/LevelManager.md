# LevelManager

Der LevelManager parst das ThemeFile und stellt die Level-Informationen bereit.

`readLevelStructure`: Level count, file offsets and Start/Stop marker validation
`loadLevel`: loads the level data from the file, emits LevelLoaded with LevelData{Bricks, Powerups, etc.}

Listens to: LevelRequested and LevelThemeRequested

## Level-Editor

- could be ingame

- LevelDesign

See: [index.htlm](../../../Website/sdl-ball.sourceforge.net/leveleditor/index.html)

## Todo

[Todo](LevelManager_Todo.md)

## See also

- [levels](../misc/level.md)
- [level_format](../misc/level_format.md)
- [Managers](../Managers.md)
- gimp level plugin [Readme.md](../../leveleditor/gimp-leveleditor/Readme.md)