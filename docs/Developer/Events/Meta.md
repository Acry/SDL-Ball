# Events

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

constructor -> registerEvents

destructor -> unregisterEvents
