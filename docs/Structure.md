# New structure

- default aus config.h - compileTime settings
- loaded - gespeicherte Settings des Spielers
- current - und die runtime settings

```text
Game
├── Init
│   ├── ConfigFileManager - LoadConfig
│   ├── SDL_Init - Displaymanager, CreateWindow, Create GL Context and State, resize
│   ├── TTF_Init - TtfLegacyGl
│   ├── Mix_Init - SoundManager - LoadSounds
│   └── IMG_Init - TextureManager - LoadTextures
│
├── Init GameManager
│   │   └── Settingsmanager -> Talks to ConfigFileManager, 
│   └── Init GameObjects
│       ├── LevelManager
│       ├── init Player
│       └── BallManager
│       ThemeManager
│
├── GameLoop / GameState
│   ├── ProcessInput()
│   │   └── HandleEvents (SDL_Events)
│   ├── Update()
│   │   ├── m_player.Update()
│   │   ├── m_ball.Update()
│   │   │   ├── Collision Detection
│   │   │   └── Position Update
│   │   └── m_bonus.Update()
│   └── Draw()
│       ├── RenderLevel
│       ├── RenderBricks
│       ├── RenderPlayer
│       └── RenderBall
```

[Managers](Managers.md)

## old

Texturen sind grundlegende Ressourcen des Rendering-Systems -> core

```text
src/
├── core/                      # Kern-Funktionalität
│   ├── display.hpp/.cpp       # Anzeige-System
│   ├── input.hpp/.cpp         # Eingabe-Verarbeitung
│   ├── sound.hpp/.cpp         # Audio-System
│   ├── init.hpp/.cpp          # Initialisierung
    └── timing/
        ├── game_timer.hpp/.cpp       # Spiel-Timing
        ├── frame_limiter.hpp/.cpp    # Frame-Begrenzung
        └── speed_controller.hpp/.cpp  # Geschwindigkeitssteuerung
│   └── config/               # Konfigurationsmanagement
│       ├── settings.hpp/.cpp  # Spieleinstellungen
│       └── config.hpp/.cpp    # Konfigurationsverwaltung
│
├── objects/                   # Spielobjekte
│   ├── object.hpp/.cpp        # Basis
│   ├── moving_object.hpp/.cpp
│   ├── ball.hpp/.cpp
│   ├── brick.hpp/.cpp
│   ├── paddle.hpp/.cpp
│   └── powerup.hpp/.cpp
│
├── managers/                  # Verwaltungsklassen
│   ├── ball_manager.hpp/.cpp
│   ├── powerup_manager.hpp/.cpp
│   └── bullet_manager.hpp/.cpp
│
├── ui/                       # Benutzeroberfläche
│   ├── menu.hpp/.cpp         # Menü-System                     x
│   ├── hud.hpp/.cpp          # Head-up Display
│   ├── title.hpp/.cpp        # Titelbildschirm                 x
│   ├── text.hpp/.cpp         # Textausgabe                     x
│   └── scoreboard.hpp/.cpp   # Punkteanzeige                   x
│
└── game/                     # Spiellogik
├── background.hpp/.cpp       # Hintergrund                     x
├── effects.hpp/.cpp          # Spezialeffekte                  x
├── highscores.hpp/.cpp       # Bestenlisten                    x
├── loadlevel.hpp/.cpp        # Level-Laden
│
└── themes/                   # Theme-System
    ├── theme_manager.hpp/.cpp # Theme-Verwaltung
    ├── theme_loader.hpp/.cpp  # Laden der Themes
    └── theme_data.hpp/.cpp    # Theme-Datenstrukturen
```
