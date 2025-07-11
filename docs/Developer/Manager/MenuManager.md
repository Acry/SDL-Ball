# MenuManager

First, I decided to completly rewrite the Menu.
Currently, I do not use any textures, like in the old code. That may change, to decorate the menu with some images or
effects.
However, for now, I want to focus on the functionality and the claim, that the game shall be runnable without any
textures.

`TAB` - toggles Menu with exclusive input
`Enter` or KpEnter selects the current item
Left go back, needs solution.

**The MenuManager is the only component that can change the settings.**
See: SettingsManager::setDefaults()

Input is exclusive, so the game does not react to any input while the menu is open.
IInputManager.h : `bool menuInputExclusive = false;`

## TODO

- [ ] Currently, I do struggle a bit with using different input methods in the menu.
- [ ] Working towards scrolling content.
- [ ] change difficulty in Menu
- [ ] HighscoreMenu ?
- [ ] LevelSelectMenu -> Gameplay

Blocker: Controller Manager
Blocker: InputMethods issue

- [ ] Controller support (events)

## Common Widgets

- Menu Labels (static text)
- Drop-down / Selectors — each setting like Model Detail, Textures, Shadows, Lighting, Effects has a left and right
  arrow to change options (e.g., High, Extra High).
- Toggles — (RadioButtons/CheckBoxes) for settings like Console, which can be On or Off.
- Sliders — for Brightness and Volume, adjustable by dragging a handle.
- Highlight Bar — a visual indicator showing which setting is currently selected (here, Shadows).
- vertical scrolling

## Thoughts

- items-struct: hover color, selected color, text color, background color?
- should I close the menu on click outside?
- Add base colors to certain menu-items?
  Orange
  Quit current game
  Leave SDL-Ball

- [ ] confirmation Dialog?

## Menu Structure - sketch

- Main Menu
    - Gameplay
        - Toggle Autoplay
        - Toggle enemies
        - Difficulty (should be toggleable, so I need to free left and right arrows)
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
        - Load Game / save game
            - Quit Current Game
    - Settings
    - Graphics
        - Effects on/off
        - Resolution
        - Fullscreen
    - Audio
        - Music Volume
        - Sound Effects Volume
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

It would be easier and faster to use an existing library, but here we are.

I have done a SDL3, shader-based OpenGL 4.6 port of [microui](https://github.com/rxi/microui/), which is a minimalistic
immediate mode UI library.

https://github.com/Immediate-Mode-UI/Nuklear

https://github.com/raysan5/raygui is a header-only library for creating GUIs in C, with 5.5k LoC and part of raylib,
which is a C library for game development: https://github.com/raysan5/raylib

### C++

https://github.com/ocornut/imgui
