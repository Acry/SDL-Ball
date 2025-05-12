# ManagerClasses

Grundsätzlich haben wir 3 Typen von Settings:
- default aus config.h - compileTime settings
- loaded - gespeicherte Settings des Spielers
- current - und die runtime settings

Manager Typen:

[SettingsManager](Manager/SettingsManager.md) spricht mit dem ConfigFileManager, hält defaults und current settings

[GameManager](Manager/GameManager.md) - GameState -> BallManager, Paddle, Bricks?

MenuManager spricht mit dem SettingsManager, GameManager, ControllerManager

## Current Managers

[BackgroundManager](Manager/BackgroundManager.md) - handles the background, loads the background file and applies it to the game
[BallManager](Manager/BallManager.md) - handles the balls, loads the ball file and applies it to the game
[ConfigFileManager](Manager/ConfigFileManager.md) - handles config pathes, evaluates and loads the config file.
[ControllerManager](Manager/ControllerManager.md) - handles the controller, and settings
[EffectManager](Manager/EffectManager.md) - handles the effects, loads the effect file and applies it to the game
[DisplayManager](Manager/DisplayManager.md) - handles the display, loads the display file and applies it to the game
[GameManager](Manager/GameManager.md) - handles the game state, loads the game and manages the game loop
[HighscoreManager](Manager/HighscoreManager.md) - handles the highscores, loads the highscores and saves them to the file.
[LevelManager](Manager/LevelManager.md) - loads the LevelOffsets, and gives the new level.
[SaveGameManager](Manager/SaveGameManager.md) - handles the savegame, loads the savegame and saves it to the file
[SettingsManager](Manager/SettingsManager.md) - handles the settings, loads the settings and saves them to the file
[SoundManager](Manager/SoundManager.md) - Loading/Cleaning and playing sounds
[TextureManager](Manager/TextureManager.md)
[ThemeManager](Manager/ThemeManager.md) - handles the themes, loads the theme file and applies it to the game
MenuManager - handles the menu, loads the menu file and applies it to the game

## Scenes

TitleScene
GameScene
GameOverScene
PauseScene
SettingsScene

See: [Structure](Structure.md)
