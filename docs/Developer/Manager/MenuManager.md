# MenuManager

TAB - open Menu with exclusive input

Typical video game settings menu.
**The MenuManager is the only component that can change the settings.**
IInputManager.h : `bool menuInputExclusive = false;`

see: SettingsManager::setDefaults()

## TODO

- [ ] change difficulty in Menu


- [ ] HighscoreMenu
- [ ] LevelSelectMenu

Blocker: Controller Manager

- [ ] Controller support (events)

## Common Widgets

- Menu Labels (static text) — e.g., GAME SETTINGS, Windowed Mode, Model Detail, etc.
- Drop-down / Selectors — each setting like Model Detail, Textures, Shadows, Lighting, Effects has a left and right
  arrow to change options (e.g., High, Extra High).
- Toggles — (RadioButtons/CheckBoxes) for settings like Console, which can be On or Off.
- Sliders — for Brightness and Volume, adjustable by dragging a handle.
- Highlight Bar — a visual indicator showing which setting is currently selected (here, Shadows).
- vertical scrolling

## Thoughts

- items-struct: hover color, selected color, text color, background color
- should I close the menu on click outside?

- Add base colors to ceratain menu-items?
  Orange
  Quit current game
  Leave SDL-Ball

- [ ] confirmation Dialog?

## Menu Structure

- Main Menu
    - Gameplay
        - Toggle Autoplay
        - Toggle enemies
        - Difficulty
            - Easy
            - Normal
            - Hard
            - Insane
        - New Game
            - restart level
            - random level
            - next level
            - previous level
            - select level
        - Load Game
        - Quit Current Game
    - Settings
    - Graphics
    - Audio
    - Controls
        - Keyboard
        - Mouse
        - Controller / Joystick
    - Themes
    - Quit

## Old Menu

![Bildschirmfoto vom 2025-05-08 15-42-26.png](../../screenshots/Bildschirmfoto%20vom%202025-05-08%2015-42-26.png)

## Existing Frameworks

### C

It would be easier and faster to use an existing library.

I have done an SDL3, shader-based OpenGL 4.6 port of [microui] https://github.com/rxi/microui/, which is a minimalistic
immediate mode UI library.

https://github.com/Immediate-Mode-UI/Nuklear

https://github.com/raysan5/raygui is a header-only library for creating GUIs in C, with 5.5k LoC and part of raylib,
which is a C library for game development: https://github.com/raysan5/raylib

### C++

https://github.com/ocornut/imgui
