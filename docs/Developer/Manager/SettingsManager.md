# SettingsManager

See: [Settings](../misc/Settings.md)

- default
- loaded
- current

Grundsätzlich haben wir 3 Typen von Settings:

`default` - aus config.h - compileTime settings
`loaded` - gespeicherte Settings des Spielers
`current` - und die runtime settings

Hält loaded, defaults und current settings.

SettingsManager nutzt ConfigFileManager

## Kandidaten, die den settingsManager nutzen

- BackgroundManager
- CodeManager
- ControllerManager
- DisplayManager
- HighscoreManager
- LevelManager
- SaveGameManager
- SceneManager
- SoundManager
- TextureManager
- ThemeManager

## Future

- add JSON support
