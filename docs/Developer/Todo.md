# Todo

Besides the obvious things like fixing bugs, refactoring and moving on SDL3 and shaders,
there are a lot of things to do.

## Pipeline

[Current things](CURRENT.md)

* GameObjects

- [X] Brick to moving object, for dropping levels
- [ ] BallManager
- [X] BrickManager
  [X] Dropping Levels
- [X] PowerupManager
- [X] ICollideable

## Refactoring

- [X] ConfigManager create
- [X] ConfigManager_Tests

- [X] SettingsManager create
- [X] SettingsManager, revert Singleton Implementation
- [X] SettingsManager_Tests

- [X] Display_Manager_Tests

- [X] Screenshot_Tests

- [X] TextureManager_Tests
- [X] TextureManager_Tests - implicitly tested in SpriteSheetAnimation_Tests

- [X] SpriteSheetAnimation, rework
- [X] SpriteSheetAnimation_Tests

- [X] ThemeManager, rework
- [X] ThemeManager_Tests (rethink the role of the ThemeManager)

- [X] TextManager, revert Singleton Implementation
- [X] TextManager_Tests

- [X] SoundManager_Tests

- [X] CollisionManager create
- [X] CollisionManager_Tests

- [X] EventHandler create
- [X] EventHandler_Tests

- [X] EffectManager, decouple
- [X] EffetManager, rework
- [X] EffectManager_Tests
- [X] EffectManager, start docs

GameObjects: Balls, Paddle, Bricks, Powerups, Tracer, etc.

- [X] Ball, rework
- [X] Ball_Tests

- [X] Tracer rework
- [X] Tracer_Tests -> Ball_Tests

- [ ] BallManager, rework
- [ ] BallManager_Tests

- [X] Paddle, rework
- [X] Paddle_Tests

- [ ] Player rework the PlayerManager is the PaddleManager
- [ ] Player_Tests

- [X] Brick, rework
- [ ] Brick_Tests
- [X] BrickManager create
- [ ] BrickManager_Tests

- [X] Powerup, rework
- [ ] Powerup_Tests
- [ ] PowerupManager, rework
- [X] PowerupManager_Tests

- [ ] Bullet, rework
- [ ] Bullet_Tests

- [ ] HUD-Manager
- HUD (Score, Clock, Lives, Level, Speedometer, Powerups consumed (bottom), Shop (top))
    - [ ] Score
    - [ ] Clock
    - [ ] Lives
    - [ ] Level
    - [ ] Powerups consumed (bottom)
    - [ ] Shop (top)
    - [ ] Speedometer
- [ ] HudManager_Tests

- [ ] Timing rework (WIP)
- [ ] Timing_Tests

___

- [ ] GameManager, create
- [ ] GameManager_Tests

- [X] CodeManager
- [ ] CodeManager_Tests

- [ ] **Game should be playable again**

___

- MenuManager, create
- [ ] fix other menus,
    - [X] MainMenu
    - [ ] SettingsMenu
    - [ ] QuitMenu
    - [ ] HighscoreMenu
    - [ ] LevelSelectMenu
- [ ] MenuManager Mouse collsion and click
- [ ] MenuManager: keyboard support (events)
- [ ] MenuManager_Tests
- [ ] InputManager_Tests

- [ ] SceneManager create
- [ ] SceneManager_Tests

## Scenes

- [ ] TitleScreen, rework
  Blocker GameManager, SceneManager, MenuManager
- [ ] TitleScreen_Tests

## Savegames

- [ ] Decouple SavegameManager
- [ ] support old savegame format 1
- [ ] support old savegame format 2
- [ ] implement current savegame format
- [ ] SavegameManager_Tests

## Highscores

- [ ] HighscoreManager_Tests

## Level

- [X] LevelManager_Tests
- [ ] LevelManager, check old formats
- [ ] modernize LevelEditor
- [ ] LevelEditor_Tests
- [ ] InGame-LevelEditor

## Auxiliary Managers

- [ ] [ShaderManager](Manager/ShaderManager.md)

## Current things

- [ ] check old code if border texture was used as spritesheet
- [X] move border checks from paddle and ball to collisionManager
- [X] fix DebugMouse.cpp, but text::write should just work. -> TestHelper
- [X] TextManager genFontTex to bool
- [ ] check early game field sizes, should I go exactly 4:3 and then render the pillars outside?
  scissor rectangle needs to be expanded to the playfield plus the pillars
- [ ] solve brick fade issue
- [X] Paddle: use playfield constants from config.h
- [ ] Many comments are in German
- [X] wird brick/doom-org.png im Spiel verwendet?

## Additional things

### Gameplay

- [ ] setting titleScreenShow ? / intro-scenes, skip all. themeSwitch-scene, asked if classic, retro or current theme

- [ ] a - restart level
- [ ] r - random level
- [ ] n - next level
- [ ] m - previous level
- [ ] f - autoplay
- [ ] c - chose level

- [ ] Make the game playable without datafiles (random levels, no samplers)
- [ ] LevelManager - random level
- [ ] simple GPU-Sounds?
- [ ] mod support for music and samples (demonstrate in Retro theme)
- [X] make game playable with keys
- [ ] MenuManager
- [ ] change difficulty in Menu
- [ ] normal Ball spritesheet - Themes: classic/retro/arkanoid/3.1 ?
- [ ] rework positional sound <https://examples.libsdl.org/SDL3/audio/05-planar-data/>
- [ ] modernize game controllers, see: https://examples.libsdl.org/SDL3/input/01-joystick-polling/
- [ ] Play with pen? https://examples.libsdl.org/SDL3/pen/01-drawing-lines/
- [ ] credits scene
- [ ] save game should save the state of the game, not just the level
- [ ] Fade in Paddle?
- [ ] Fade in Ball?
- [ ] Animate Paddle texture
- [ ] Animate Glue layer
- [X] [SpriteSheetAnimation](misc/SpriteSheetAnimation.md) einmaliges Abspielen implementieren
- [ ] fix speedometer

## Themes

- [Arkanoid ](Themes/Araknoid.md) smalle playfield
- Retro [Theme](Themes/Retro.md)
- Ingame (static), minimalitic

#### Tracer

- [ ] Tracer, to EffectManager
- [ ] tracer shows red color and shows up when glued to the paddle, should reset in new level
- [ ] is tracer speed dependent on the speed of the ball?
- [ ] Tracer, if Ball explosive, make Tracer more orange and dont use Colors of bricks bounced to.
- [ ] Tracer refactoring (effectManager/eventManager/inheritance/particleSystem/consistent naming, from LevelTheme to
  instance)
  Named: Tail, Tracer, BallEffect, BallTracer, BallTail ...
  public: tracer tail; - andersrum wäre richtig gewesen. Tracer ist eine Form des Ball-Tails
- [ ] Tracer, inherit from growable
- [X] Tracer, inherit from MovingObject

### Auxiliary

- [ ] use OpenGL debug callbacks - think it is not worth for legcy gl
- [X] I broke the screenshot feature after extraction. Might be a threading issue.
  Not sure, think it was because the method was static
- [ ] ScreenshotManager with UI?
- [ ] use player name from config
- [ ] savegame folder, use textfile instead of binary

- [ ] fix messed up values due to [projection change](OpenGL/Projection.md).
- [ ] use sdl3 high performance timer, see: [Timing](Physics%20&%20Timing/Timing.md)
- [ ] Refactor [Timing](Refactoring/Timing.md)
- [ ] show current level no# during gameplay, IDK get ready for level X, fade in paddle and ball, play sounds
- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] open website from title screen
- [ ] add a new scene for the used keys
- [ ] add a new scene for more explanation, see Website page=about.html
- [ ] autoplay after title for a while
- [ ] autosave level on quit, asked for continuing
- [ ] hot reload themes: levels, backgrounds graphics, sounds, fonts
- [ ] polish JS Leveleditor?
- [X] check gimp level plugin [Readme.md](../../leveleditor/gimp-leveleditor/Readme.md)
- [ ] maintain gimp level plugin? IDK
- [ ] using a physics engine at this point might be overkill.
- [ ] add a pause indicator / Pause Scene
- [ ] rework the powerup shop
- [ ] https://examples.libsdl.org/SDL3/demo/04-bytepusher/

- [X] implement callbacks (EventManager)

- [ ] use shaders
- [ ] use SDL 3, not sure if I should maintain SDL2 plus Emscripten
- [ ] Emscripten
- [ ] ThemeManager zu cmdline, um Themes zu validieren?

### Source Structure

Blocker: implement all managers
Blocker: arrange Managers in categories

- [ ] move all GameObjects to GameObjects folder
- [ ] move all Managers to Manager folder
- [ ] move all Scenes to Scene folder

### Tests

- [ ] Makefile is getting huge, clion has already problems
- [X] write used keys as help, with TextManager, if graphics are available
- [X] move all tests to the test folder
- [X] separate manual tests from automatic tests
- [ ] write automatic tests
- [ ] add Theme fixtures to tests
- [ ] add saveGames fixtures to tests
- [ ] add Level fixtures to tests
- [ ] add Highscore fixtures to tests

## Bugs

- [ ] [Tracer](GameObjects/Tracer.md) ist shortly visible after ball reset
