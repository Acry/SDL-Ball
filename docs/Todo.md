# Todo

Besides the obvious things like fixing bugs, refactoring and moving on to shaders,
there are a lot of things to do.

ConfigFileManager configFileManager;
SettingsManager settingsManager(configFileManager);

- [X] ConfigFileManager_Tests
- [X] SettingsManager, revert Singleton Implementation]
- [X] SettingsManager_Tests
- [X] Display_Manager_Tests
- [X] Screenshot_Tests
- [X] TextureManager_Tests
- [X] BackgroundManager_Tests

- [ ] ThemeManager_Tests (rethink the role of the ThemeManager)
- [ ] SoundManager_Tests
- [ ] TextureAnimation_Tests
- [ ] EffectManager_Tests
- [ ] TitleScreen_Tests

- [ ] check early game field sizes, should I go exactly 4:3 and then render the pillars outside?
      scissor rectangle needs to be expanded to the playfield plus the pillars
- [ ] TextManager_Tests (not sure if this component should be merged with ConfigFileManager)

- [ ] SavegameManager_Tests
- [ ] LevelManager_Tests
- [ ] LevelEditor_Tests
- [ ] HighscoreManager_Tests
- [ ] GameManager_Tests

- HUD (Score, Lives, Level, Speedometer, Powerups, Shop)
- [ ] fix SpeedoMeter and write Test
hm, GameObjects: Balls, Paddles, Bricks, Powerups, Tracers, etc.

- [ ] BallManager_Tests
- [ ] BrickManager_Tests

Here is a list of things that I want to do.

- [ ] implement the tainting of the background, with a reasonable mix (blend) function and settings
- [ ] Make game playable withouth datafiles (random levels, no samplers)
- [ ] simple GPU-Sounds?
- [ ] use playername from config
- [ ] savegame folder, use textfile instead of binary
- [ ] use OpenGL debug callbacks
- [ ] solve brick fade issue
- [ ] fix speedometer
- [ ] fix messed up values due to [projection change](Refactoring/Projection.md).
- [ ] write tests
- [ ] Refactor [Timing](Refactoring/Timing.md)
- [ ] check save game structure -> binary, whyever. Should be name and level.
- [ ] show current level no.  during gameplay
- [ ] a - restart level
- [ ] r - random level
- [ ] n - next level
- [ ] m - previous level
- [ ] f - autoplay
- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] add a new scene for the used keys
- [ ] add a new scene for more explainations, see Website page=about.html
- [ ] Think, I broke the screenshot feature, after extraction. Might be a threading issue.
- [ ] autoplay after title for a while
- [ ] autosave level on quit, asked for continue
- [ ] hot reload themes/levels/graphics/sounds
- [ ] polish Leveleditor
- [ ] using a physics engine at this point might be an overkill.
- [ ] add a pause indicator
- [ ] rework the shop
- [ ] https://examples.libsdl.org/SDL3/demo/04-bytepusher/

- [ ] implement callbacks

- [ ] use shaders
- [ ] use SDL 3, not sure if i should maintain SDL2 plus Emscripten
- [ ] Emscripten

## Input

### Keyboard control

- make game playable with keys

### Controllers

modernize, see:
https://examples.libsdl.org/SDL3/input/01-joystick-polling/

Play with pen?
https://examples.libsdl.org/SDL3/pen/01-drawing-lines/

## Settings

- [ ] titleScreenShow
- [ ] background : taint_bg_color

## Classes

- [ ] ScreenshotManager with UI?

## Update

- [ ] use sdl2 highperformance timer
see: [Timing](Refactoring/Timing.md)

## Menu

- [ ] fix projection for submenus
- [ ] change difficulty

## Idle mode

- [ ] let KI play for a while

## Title

- show keys and powerups interchanged

## Ball-rendering

- add normal texture atlas
- tracer show red color and shows up when glued to the paddle
- is tracer speed dependent on the speed of the ball?

## Audio

rework positional sound
https://examples.libsdl.org/SDL3/audio/05-planar-data/
