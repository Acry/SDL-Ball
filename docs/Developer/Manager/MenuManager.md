# MenuManager

![Bildschirmfoto vom 2025-05-08 15-42-26.png](../../screenshots/Bildschirmfoto%20vom%202025-05-08%2015-42-26.png)

TAB
emit event MenuOpened -> pause game
emit event MenuClosed -> resume game

- [ ] create MenuStructure and flow
- [ ] MenuManager
- [ ] change difficulty in Menu

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

**The MenuManager is the only compnonent that can change the settings.**

Restart level

- [ ] a - restart level
- [ ] r - random level
- [ ] n - next level
- [ ] m - previous level
- [ ] f - autoplay
- [ ] c - choose level

New game

Orange
Quit current game
Leave SDL-Ball

Settings

## events

using KeyboardEventCallback = std::function<void(const KeyboardEventData &)>;