# Todo

- [ ] tracer ist shortly visible after ball reset

Besides the obvious things like fixing bugs, refactoring and moving on to shaders,
there are a lot of things to do.

## Refactoring

- [X] ConfigFileManager_Tests
- [X] SettingsManager, revert Singleton Implementation
- [X] SettingsManager_Tests
- [X] Display_Manager_Tests
- [X] Screenshot_Tests
- [X] TextureManager_Tests
- [X] BackgroundManager_Tests
- [X] TextManager_Tests - implicitly tested in TextureAnimation_Tests
- [X] TextureAnimation_Tests
- [X] ThemeManager_Tests (rethink the role of the ThemeManager)
- [X] TextManager, revert Singleton Implementation
- [X] TextManager_Tests
- [X] SoundManager_Tests
- [X] CollisionManager_Tests
- [X] EventHandler_Tests

- [ ] Decouple SavegameManager
- [ ] support old savegame format 1
- [ ] support old savegame format 2
- [ ] implement current savegame format
- [ ] SavegameManager_Tests

- [ ] LevelManager_Tests
- [ ] LevelManager, check old formats
- [ ] HighscoreManager_Tests
- [ ] den EffectManager von den Spielobjekten zu entkoppeln
- [ ] EffectManager_Tests
- [ ] TitleScreen_Tests

- HUD (Score, Clock, Lives, Level, Speedometer, Powerups consumed (bottom), Shop (top))
  - [ ] Score
  - [ ] Clock
  - [ ] Lives
  - [ ] Level
  - [ ] Powerups consumed (bottom)
  - [ ] Shop (top)
  - [ ] Speedometer
- [ ] HudManager_Tests

hm, GameObjects: Balls, Paddles, Bricks, Powerups, Tracers, etc.
- [ ] BallManager_Tests
- [ ] BrickManager_Tests
- [ ] Player_Tests
- [X] Ball_Tests

- [ ] GameObject_Tests
- [ ] GameObjectManager_Tests
- [ ] Timing_Tests
- [ ] GameManager erstellen
- [ ] GameManager_Tests

- [ ] **Game should be playable again**
- [ ] fix other menus,
  - [X] MainMenu
  - [ ] SettingsMenu
  - [ ] QuitMenu
  - [ ] HighscoreMenu
  - [ ] LevelSelectMenu
- [ ] MenuManager: keyboard support
- [ ] MenuManager_Tests
- [ ] InputManager_Tests
- [ ] modernize LevelEditor
- [ ] LevelEditor_Tests
- [ ] [ShaderManager](Manager/ShaderManager.md)

## Currents things

- [ ] move border checks from paddle and ball to collisionManager
- [ ] fix DebugMouse.cpp, but text::write should just work.
- [ ] TextManager genFontTex to bool
- [ ] check early game field sizes, should I go exactly 4:3 and then render the pillars outside?
  scissor rectangle needs to be expanded to the playfield plus the pillars
- [ ] solve brick fade issue
- [ ] Tests: manual tests should reside in the test folder, not in the src folder
- [ ] add Theme fixtures to tests
- [ ] add saveGames to tests
- [ ] Paddle: use playfield constants from config.h

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
- [ ] make game playable with keys
- [ ] fix projection for submenus
- [ ] change difficulty
- [ ] normal Ball spritesheet
- [ ] tracer shows red color and shows up when glued to the paddle, should reset in new level
- [ ] is tracer speed dependent on the speed of the ball?
- [ ] Tracer, if Ball explosive, make Tracer more orange and dont use Colors of bricks bounced to.
- [ ] rework positional sound <https://examples.libsdl.org/SDL3/audio/05-planar-data/>
- [ ] modernize game controllers, see: https://examples.libsdl.org/SDL3/input/01-joystick-polling/
- [ ] Play with pen? https://examples.libsdl.org/SDL3/pen/01-drawing-lines/
- [ ] use sdl2 highperformance timer, see: [Timing](Refactoring/Timing.md)
- [ ] credits scene
- [ ] save game should save the state of the game, not just the level
- [ ] Tracer refactoring (effectManager/eventManager/inheritance/particleSystem/consitent naming, from LevelTheme to instance)
      Named: Tail, Tracer, BallEffect, BallTracer, BallTail ...
        public: tracer tail; - andersrum wäre richtig gewesen. Tracer ist eine Form des Ball-Tails

### Auxiliary

- [X] I broke the screenshot feature, after extraction. Might be a threading issue.
  Not sure, think it was because the method was static
- [ ] ScreenshotManager with UI?
- [ ] use player name from config
- [ ] savegame folder, use textfile instead of binary
- [ ] use OpenGL debug callbacks

- [ ] fix speedometer
- [ ] fix messed up values due to [projection change](OpenGL/Projection.md).
- [ ] write tests
- [ ] Refactor [Timing](Refactoring/Timing.md)
- [ ] show current level no. during gameplay

- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] open website from title screen
- [ ] add a new scene for the used keys
- [ ] add a new scene for more explanation, see Website page=about.html

- [ ] autoplay after title for a while
- [ ] autosave level on quit, asked for continuing
- [ ] hot reload themes/levels/graphics/sounds
- [ ] polish Leveleditor
- [ ] using a physics engine at this point might be overkill.
- [ ] add a pause indicator / Pause Scene
- [ ] rework the shop
- [ ] https://examples.libsdl.org/SDL3/demo/04-bytepusher/

- [ ] implement callbacks

- [ ] use shaders
- [ ] use SDL 3, not sure if I should maintain SDL2 plus Emscripten
- [ ] Emscripten
- [ ] ThemeManager zu cmdline, um Themes zu validieren


### Tests

- [ ] write used keys as help, with TextManager, if graphics are available
