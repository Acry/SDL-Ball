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
eventManager->addListener(GameEvent::EventType,
                          [this](const DataType &data) { callBack(data); }, this);
```

Unregister

```c++
eventManager->removeListener(GameEvent::EventType, this);
```

Emit

```c++
eventManager->emit(GameEvent::EventType, data);
```
