# InputManager

Interface `IInputManager`

`MouseManager`, `KeyboardManager`, JoystickManager tbd, [ControllerManager](ControllerManager.md)

Der `InputDispatcher` (bzw. Event\-Dispatcher oder Input\-Controller) sollte den aktuellen `GameState` abfragen und die
Eingaben entsprechend weiterleiten oder ignorieren.  
Dadurch wird sichergestellt, dass z\.B\. im Menü keine Spielfunktionen ausgelöst werden und die Eingaben immer
kontextsensitiv verarbeitet werden.

**Kurz:**  
Der `InputDispatcher` prüft vor der Verarbeitung eines Inputs den aktuellen `GameState` und entscheidet, welche Manager
oder Logik darauf reagieren dürfen.

## See also

[Input](../Input/Intro.md)

## Future

- [ ] Modernize joystick code, see: https://examples.libsdl.org/SDL3/input/01-joystick-polling/
- [ ] Play with pen? https://examples.libsdl.org/SDL3/pen/01-drawing-lines/
