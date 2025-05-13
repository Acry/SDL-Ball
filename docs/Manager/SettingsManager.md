# SettingsManager

- default
- loaded
- current

Grundsätzlich haben wir 3 Typenvon Settings:

`default` - aus config.h - compileTime settings
`loaded` - gespeicherte Settings des Spielers
`current` - und die runtime settings

Hält defaults und current settings.

SettingsManager nutzt ConfigFileManager 
Kandidaten die den settingsManager nutzen:
- GameManager
- DisplayManager
- SoundManager
- ThemeManager
- ControllerManager
- BackgroundManager
- EffectManager
- LevelManager
- MenuManager

Weitere 
See: [Settings](Settings.md)
See: [Managers](Managers.md)
