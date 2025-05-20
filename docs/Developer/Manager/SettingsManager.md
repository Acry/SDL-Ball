# SettingsManager

- default
- loaded
- current

Grundsätzlich haben wir 3 Typen von Settings:

`default` - aus config.h - compileTime settings
`loaded` - gespeicherte Settings des Spielers
`current` - und die runtime settings

Hält loaded, defaults und current settings.

SettingsManager nutzt ConfigFileManager 

Kandidaten die den settingsManager nutzen:
- GameManager

Momentan vermute ich, dass es das beste wäre, wenn nur der GameManager mit dem SettingsManager redet.

- DisplayManager
- SoundManager
- ThemeManager
- ControllerManager
- BackgroundManager
- EffectManager
- LevelManager
- MenuManager

Weitere 
See: [Settings](../misc/Settings.md)
See: [Managers](../Managers.md)
