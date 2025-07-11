# Input

Die SDL2-API unterscheidet zwischen Joystick (`SDL_Joystick`-API) und Gamecontroller (`SDL_GameController`-API):

`SDL_Joystick`: Für generische Joysticks, alle Eingabegeräte werden als Achsen, Buttons und Hats behandelt.
`SDL_GameController`: Für unterstützte Gamepads (z.B. Xbox, PlayStation), mit standardisiertem Button/Axis-Mapping und
komfortablerer API.

## PS5-Controller

Um einen PS5-Controller (DualSense) mit SDL2 zu testen, solltest du die SDL_GameController-API verwenden, da der
Controller von SDL2 als Gamecontroller erkannt wird. Damit kannst du standardisierte Events für Buttons und Achsen
abfragen.

Kurzes Beispiel für die Initialisierung und Event-Verarbeitung:

see `ControllerManager_Tests.cpp`

Die Events wie `SDL_CONTROLLERBUTTONDOWN` findest du in der SDL2-Headerdatei SDL_events.h.

Sie sind als Konstanten (z.B. `#define SDL_CONTROLLERBUTTONDOWN 0x650`) definiert und werden von SDL beim Polling von
Events (SDL_PollEvent) im Feld event.type gesetzt.

Weitere relevante Controller-Events sind z.B.:

SDL_CONTROLLERBUTTONUP
SDL_CONTROLLERAXISMOTION
SDL_CONTROLLERDEVICEADDED
SDL_CONTROLLERDEVICEREMOVED

Die zugehörigen Strukturen findest du ebenfalls in den SDL2-Headern, z.B. SDL_ControllerButtonEvent für Button-Events.

`SDL_events.h`

```c++
    /* Game controller events */
    SDL_CONTROLLERAXISMOTION  = 0x650, /**< Game controller axis motion */
    SDL_CONTROLLERBUTTONDOWN,          /**< Game controller button pressed */
    SDL_CONTROLLERBUTTONUP,            /**< Game controller button released */
    SDL_CONTROLLERDEVICEADDED,         /**< A new Game controller has been inserted into the system */
    SDL_CONTROLLERDEVICEREMOVED,       /**< An opened Game controller has been removed */
    SDL_CONTROLLERDEVICEREMAPPED,      /**< The controller mapping was updated */
    SDL_CONTROLLERTOUCHPADDOWN,        /**< Game controller touchpad was touched */
    SDL_CONTROLLERTOUCHPADMOTION,      /**< Game controller touchpad finger was moved */
    SDL_CONTROLLERTOUCHPADUP,          /**< Game controller touchpad finger was lifted */
    SDL_CONTROLLERSENSORUPDATE,        /**< Game controller sensor was updated */
    SDL_CONTROLLERUPDATECOMPLETE_RESERVED_FOR_SDL3,
    SDL_CONTROLLERSTEAMHANDLEUPDATED,  /**< Game controller Steam handle has changed */
```
