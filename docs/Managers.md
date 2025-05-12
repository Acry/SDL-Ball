# ManagerClasses

Grundsätzlich haben wir 3 Typen:
- default aus config.h - compileTime settings
- loaded - gespeicherte Settings des Spielers
- current - und die runtime settings

## Current Managers

[BackgroundManager](BackgroundManager.md) - handles the background, loads the background file and applies it to the game
[BallManager](BallManager.md) - handles the balls, loads the ball file and applies it to the game
[ConfigFileManager](ConfigFileManager.md) - handles config pathes, evaluates and loads the config file.
[ControllerManager](ControllerManager.md) - handles the controller, and settings
[EffectManager](EffectManager.md) - handles the effects, loads the effect file and applies it to the game
[DisplayManager](DisplayManager.md) - handles the display, loads the display file and applies it to the game
[GameManager](GameManager.md) - handles the game state, loads the game and manages the game loop
[HighscoreManager](HighscoreManager.md) - handles the highscores, loads the highscores and saves them to the file.
[LevelManager](LevelManager.md) - loads the LevelOffsets, and gives the new level.
[SettingsManager](SettingsManager.md) - handles the settings, loads the settings and saves them to the file
[SoundManager](SoundManager.md) - Loading/Cleaning and playing sounds
[TextureManager](TextureManager.md)
[ThemeManager](ThemeManager.md) - handles the themes, loads the theme file and applies it to the game
MenuManager - handles the menu, loads the menu file and applies it to the game
SaveGameManager

## Scenes

TitleScene
GameScene
GameOverScene
PauseScene
SettingsScene

See: [Structure](Structure.md)
