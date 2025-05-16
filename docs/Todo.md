# Todo

Besides the obvious things like fixing bugs, refactoring and moving on to shaders,
there are a lot of things to do.

Here is a list of things that I want to do.

- [ ] solve brick fade issue
- [ ] fix speedometer
- [ ] fix messed up values due to [projection change](Refactoring/Projection.md).
- [ ] write tests
- [ ] apply Meyer's Singleton pattern to manager classes.
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
- [ ] Emscripten
- [ ] implement callbacks
- [ ] use OpenGL debug callbacks

## Keyboard control

- make game playable with keys

## Controllers

modernize, see:
https://examples.libsdl.org/SDL3/input/01-joystick-polling/

Play with pen?
https://examples.libsdl.org/SDL3/pen/01-drawing-lines/

## Settings

- titleScreenShow

## Classes

- ScreenshotManager with UI?

## Update

- use sdl2 highperformance timer
see: [Timing](Timing.md)

## Menu

 change difficulty

## Idle mode

- let KI play for a while

## Title

- show keys and powerups interchanged

## Ball-rendering

- tracer show red color and shows up when glued to the paddle
- is tracer speed dependent on the speed of the ball?

## Audio

rework positional sound
https://examples.libsdl.org/SDL3/audio/05-planar-data/
