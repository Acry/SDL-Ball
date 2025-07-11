# ThemeManager Todo

[ThemeManager](ThemeManager.md)

Themes: Classic / Retro / 3.1 / Arkanoid / Internal theme

## current

ThemeManager::initThemeRoots() should use config gameName internally, not hardcoded, like "sdl-ball-remastered"

### Classic Theme

- [ ] Bring back classic border
- [ ] Bring back classic paddle

## Future

### Playfield borders

- [ ] new global theme description files
  I deleted the playfield border-test, that was not clever, it was there for a reason.
  I was working towards dynamic borders, e.g., dropping levels.
  Also, a theme should define the playfield border, so it can be different for each theme.
  Arkanoid needs a smaller playfield.
  We can add theme metadata, like [enemies](../Physics%20&%20Timing/enemies.md) , true/false and different difficulties.
  Life, Ballspeed, Ball size, Paddle size, Points, powerup selection, etc. is all theme related.
  describes the theme, author, version, etc.
  can point to folders with the theme files
  has some global settings: enemies or not, difficulty settings

- [ ] ThemeManager to cmdline, validate themes?

### Retro Theme (Clean Sweep)?

- Retro [Theme](Themes/Retro.md)
- [ ] simple GPU-Sounds? (SoundManager_Todo.md)
  [See:](../../Game/Intro.md) Clean Sweep, Breakout, Arkanoid, DX-Ball, Megaball, Shatter, Ricochet

### Modern Theme

- [ ] Normal Ball spritesheet
- [ ] Animate Paddle texture
- [ ] Animate Glue layer

### Internal Theme

Claim: The game should run without any theme, no texture, no sound, no levels, no backgrounds.

- [ ] Make the game playable without datafiles (random levels, no samplers)
- [ ] fill-colors or shaders
- [ ] In game (static), minimalistic
- [ ] sound->gpu or internal mods, graphics (xpm)?

### Arkanoid Theme

- [Arkanoid ](Themes/Araknoid.md) smaller playfield