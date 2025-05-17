# Todo

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
- [ ] TextManager_Tests
- [ ] SoundManager_Tests
- [ ] SavegameManager_Tests
- [ ] LevelManager_Tests
- [ ] HighscoreManager_Tests

- [ ] den EffectManager von den Spielobjekten zu entkoppeln
- [ ] EffectManager_Tests

- [ ] TitleScreen_Tests

- [ ] InputManager_Tests
- [ ] GameObject_Tests
- [ ] GameObjectManager_Tests
- [ ] LevelEditor_Tests

- [ ] MenuManager_Tests
- [ ] HudManager_Tests

- [ ] Timing_Tests
- [ ] GameManager_Tests

- HUD (Score, Clock, Lives, Level, Speedometer, Powerups consumed (bottom), Shop (top))
  - [ ] Score
  - [ ] Clock
  - [ ] Lives
  - [ ] Level
  - [ ] Powerups consumed (bottom)
  - [ ] Shop (top)
  - [ ] Speedometer

hm, GameObjects: Balls, Paddles, Bricks, Powerups, Tracers, etc.
- [ ] BallManager_Tests
- [ ] BrickManager_Tests
- [ ] Player_Tests
- [ ] Ball_Tests

- [ ] **Game should be playable again**

## Currents things

- [ ] check early game field sizes, should I go exactly 4:3 and then render the pillars outside?
  scissor rectangle needs to be expanded to the playfield plus the pillars
- [ ] solve brick fade issue

## Additional things

### Gameplay

- [ ] setting titleScreenShow
- [ ] implement the tainting of the background, with a reasonable mix (blend) function and settings
- [ ] a - restart level
- [ ] r - random level
- [ ] n - next level
- [ ] m - previous level
- [ ] f - autoplay
- [ ] Make game playable withouth datafiles (random levels, no samplers)
- [ ] simple GPU-Sounds?
- [ ] make game playable with keys
- [ ] fix projection for submenus
- [ ] change difficulty
- [ ] normal Ball spritesheet
- [ ] tracer shows red color and shows up when glued to the paddle, should reset in new level
- [ ] is tracer speed dependent on the speed of the ball?
- [ ] rework positional sound <https://examples.libsdl.org/SDL3/audio/05-planar-data/>
- [ ] modernize game controllers, see: https://examples.libsdl.org/SDL3/input/01-joystick-polling/
- [ ] Play with pen? https://examples.libsdl.org/SDL3/pen/01-drawing-lines/
- [ ] use sdl2 highperformance timer, see: [Timing](Refactoring/Timing.md)

### Auxiliary

- [X] I broke the screenshot feature, after extraction. Might be a threading issue.
  Not sure, think it was because the method was static
- [ ] ScreenshotManager with UI?
- [ ] use player name from config
- [ ] savegame folder, use textfile instead of binary
- [ ] use OpenGL debug callbacks

- [ ] fix speedometer
- [ ] fix messed up values due to [projection change](Refactoring/Projection.md).
- [ ] write tests
- [ ] Refactor [Timing](Refactoring/Timing.md)
- [ ] check save game structure -> binary, whyever. Should be name and level.
- [ ] show current level no.  during gameplay

- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] open website from title screen
- [ ] add a new scene for the used keys
- [ ] add a new scene for more explainations, see Website page=about.html

- [ ] autoplay after title for a while
- [ ] autosave level on quit, asked for continue
- [ ] hot reload themes/levels/graphics/sounds
- [ ] polish Leveleditor
- [ ] using a physics engine at this point might be an overkill.
- [ ] add a pause indicator / Pause Scene
- [ ] rework the shop
- [ ] https://examples.libsdl.org/SDL3/demo/04-bytepusher/

- [ ] implement callbacks

- [ ] use shaders
- [ ] use SDL 3, not sure if i should maintain SDL2 plus Emscripten
- [ ] Emscripten
- [ ] ThemeManager zu cmdline um Themes zu validieren
