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

[ ] Einige Bricks laufen in späteren rechts aus dem Bild, Level: 15, 18, 20, 35
[ ] add Bricks per row vaildation
[ ] I think those levels need to be fixed. Some Bricks are under the Playfield border.

## See also

- [levels](../misc/level.md)
- [level_format](../misc/level_format.md)
- [Managers](../Managers.md)
