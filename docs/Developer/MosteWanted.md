# Most wanted patterns

## Dependency Injection

```c++
#pragma once

class IEventManager; // Forward-Deklaration

class TextManager {
    IEventManager* eventManager;
    // ...
public:
    explicit TextManager(IEventManager* evtMgr);
    // ...
};
```

```c++
TextManager::TextManager(IEventManager* evtMgr)
    : eventManager(evtMgr) {
    // ...
}
```

## Events

Register

```c++
eventManager->addListener(GameEvent::LevelLoaded,
                          [this](const LevelData &data) { handleLevelLoaded(data); }, this);
```

Unregister

```c++
TextManager::~TextManager() {
    if (eventManager) {
        eventManager->removeListener(GameEvent::FontThemeRequested, this);
    }
```

```c++
    eventManager.addListener(
        GameEvent::FontThemeChanged,
        [&textManager](const ThemeData &data) {
            textManager.addAnnouncement("New Font Theme", 2000, Fonts::AnnounceGood);
        },
        nullptr
    );
```
