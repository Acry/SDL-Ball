# Todo

Besides the obvious things like fixing bugs, refactoring and moving on to shaders,
there are a lot of things to do.

## Pipeline

* GameObjects
- [ ] BallManager
- [ ] BrickManager
- [ ] PowerupManager
- [X] ICollideable

## Refactoring

- [X] ConfigManager create
- [X] ConfigManager_Tests

- [X] SettingsManager create
- [X] SettingsManager, revert Singleton Implementation
- [X] SettingsManager_Tests

- [X] Display_Manager_Tests

- [X] Screenshot_Tests

- [X] BackgroundManager_Tests

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

- [ ] Brick, rework
- [ ] BrickManager create
- [ ] BrickManager_Tests

- [ ] Bullet, rework
  Blockers: Bricks, (decouple -> CollisionManager, EventManager)
- [ ] Bullet_Tests

- [ ] Powerup, rework
  Blocker: BallManager
- [ ] Powerup_Tests
- [ ] PowerupManager, rework
- [ ] PowerupManager_Tests

- HUD (Score, Clock, Lives, Level, Speedometer, Powerups consumed (bottom), Shop (top))
  - [ ] Score
  - [ ] Clock
  - [ ] Lives
  - [ ] Level
  - [ ] Powerups consumed (bottom)
  - [ ] Shop (top)
  - [ ] Speedometer
- [ ] HudManager_Tests

- [ ] Player rework
- [ ] Player_Tests

- [ ] Timing rework (WIP)
- [ ] Timing_Tests

___

- [ ] GameManager, create
- [ ] GameManager_Tests

- [ ] **Game should be playable again**

___

- MenuManager, create
- [ ] fix other menus,
  - [X] MainMenu
  - [ ] SettingsMenu
  - [ ] QuitMenu
  - [ ] HighscoreMenu
  - [ ] LevelSelectMenu
- [ ] MenuManager: keyboard support
- [ ] MenuManager_Tests
- [ ] InputManager_Tests

- [ ] SceneManager create
- [ ] SceneManager_Tests

- [ ] TitleScreen, rework
      Blocker GameManager, SceneManager, MenuManager
- [ ] TitleScreen_Tests

- [ ] Decouple SavegameManager
- [ ] support old savegame format 1
- [ ] support old savegame format 2
- [ ] implement current savegame format
- [ ] SavegameManager_Tests

- [ ] LevelManager_Tests
- [ ] LevelManager, check old formats

- [ ] HighscoreManager_Tests

- [ ] modernize LevelEditor
- [ ] LevelEditor_Tests

- [ ] [ShaderManager](Manager/ShaderManager.md)

## Currents things

- [ ] check old code if border texture was used as spritesheet
- [ ] move border checks from paddle and ball to collisionManager
- [ ] fix DebugMouse.cpp, but text::write should just work.
- [X] TextManager genFontTex to bool
- [ ] check early game field sizes, should I go exactly 4:3 and then render the pillars outside?
  scissor rectangle needs to be expanded to the playfield plus the pillars
- [ ] solve brick fade issue
- [X] Paddle: use playfield constants from config.h
- [ ] Many comments are in German
- [ ] wird brick/doom-org.png im Spiel verwendet?
## Additional things

### Gameplay

- [ ] make BackgroundManager-Shader responsive to events
- [ ] setting titleScreenShow
- [ ] implement the tainting of the background, with a reasonable mix (blend) function and settings

- [ ] a - restart level
- [ ] r - random level
- [ ] n - next level
- [ ] m - previous level
- [ ] f - autoplay

- [ ] Make the game playable without datafiles (random levels, no samplers)
- [ ] simple GPU-Sounds?
- [ ] create reandom levels

- [X] make game playable with keys
- [ ] fix projection for submenus
- [ ] change difficulty
- [ ] normal Ball spritesheet
- [ ] rework positional sound <https://examples.libsdl.org/SDL3/audio/05-planar-data/>
- [ ] modernize game controllers, see: https://examples.libsdl.org/SDL3/input/01-joystick-polling/
- [ ] Play with pen? https://examples.libsdl.org/SDL3/pen/01-drawing-lines/
- [ ] use sdl2 highperformance timer, see: [Timing](Physics%20&%20Timing/Timing.md)
- [ ] credits scene
- [ ] save game should save the state of the game, not just the level
- [ ] Fade in Paddle
- [ ] Fade in Ball
- [ ] Animate Paddle texture
- [ ] Animate Glue layer
- [ ] [SpriteSheetAnimation](misc/SpriteSheetAnimation.md) einmaliges Abspielen implementieren

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

- [ ] use OpenGL debug callbacks
- [X] I broke the screenshot feature, after extraction. Might be a threading issue.
  Not sure, think it was because the method was static
- [ ] ScreenshotManager with UI?
- [ ] use player name from config
- [ ] savegame folder, use textfile instead of binary
- [ ] fix speedometer
- [ ] fix messed up values due to [projection change](OpenGL/Projection.md).
- [ ] Refactor [Timing](Refactoring/Timing.md)
- [ ] show current level no# during gameplay
- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] open website from title screen
- [ ] add a new scene for the used keys
- [ ] add a new scene for more explaination, see Website page=about.html
- [ ] autoplay after title for a while
- [ ] autosave level on quit, asked for continuing
- [ ] hot reload themes/levels/graphics/sounds
- [ ] polish Leveleditor
- [ ] using a physics engine at this point might be overkill.
- [ ] add a pause indicator / Pause Scene
- [ ] rework the shop
- [ ] https://examples.libsdl.org/SDL3/demo/04-bytepusher/

- [X] implement callbacks (EventManager)

- [ ] use shaders
- [ ] use SDL 3, not sure if I should maintain SDL2 plus Emscripten
- [ ] Emscripten
- [ ] ThemeManager zu cmdline, um Themes zu validieren

### Tests

- [ ] write used keys as help, with TextManager, if graphics are available
- [X] move all tests to the test folder
- [ ] seprate manual tests from automatic tests
- [ ] write automatic tests
- [ ] add Theme fixtures to tests
- [ ] add saveGames fixtures to tests
- [ ] add Level fixtures to tests
- [ ] add Highscore fixtures to tests

## Bugs

- [ ] [Tracer](GameObjects/Tracer.md) ist shortly visible after ball reset
