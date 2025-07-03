Die SDL2-API unterscheidet zwischen Joystick (SDL_Joystick-API) und Gamecontroller (SDL_GameController-API):

SDL_Joystick: Für generische Joysticks, alle Eingabegeräte werden als Achsen, Buttons und Hats behandelt.
SDL_GameController: Für unterstützte Gamepads (z.B. Xbox, PlayStation), mit standardisiertem Button/Axis-Mapping und
komfortablerer API.

## PS5-Controller

Um einen PS5-Controller (DualSense) mit SDL2 zu testen, solltest du die SDL_GameController-API verwenden, da der
Controller von SDL2 als Gamecontroller erkannt wird. Damit kannst du standardisierte Events für Buttons und Achsen
abfragen.

Kurzes Beispiel für die Initialisierung und Event-Verarbeitung:

```c++
#include <SDL.h>
#include <iostream>

int main() {
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    // Öffne den ersten erkannten Gamecontroller
    SDL_GameController *controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                std::cout << "Controller gefunden: " << SDL_GameControllerName(controller) << std::endl;
                break;
            }
        }
    }

    if (!controller) {
        std::cout << "Kein Gamecontroller gefunden!" << std::endl;
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                std::cout << "Button gedrückt: " << (int)event.cbutton.button << std::endl;
            }
            if (event.type == SDL_CONTROLLERAXISMOTION) {
                std::cout << "Achse: " << (int)event.caxis.axis << " Wert: " << event.caxis.value << std::endl;
            }
        }
    }

    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}
```

Die Events wie SDL_CONTROLLERBUTTONDOWN findest du in der SDL2-Headerdatei SDL_events.h.

Sie sind als Konstanten (z.B. #define SDL_CONTROLLERBUTTONDOWN 0x650) definiert und werden von SDL beim Polling von
Events (SDL_PollEvent) im Feld event.type gesetzt.

Weitere relevante Controller-Events sind z.B.:

SDL_CONTROLLERBUTTONUP
SDL_CONTROLLERAXISMOTION
SDL_CONTROLLERDEVICEADDED
SDL_CONTROLLERDEVICEREMOVED
Die zugehörigen Strukturen findest du ebenfalls in den SDL2-Headern, z.B. SDL_ControllerButtonEvent für Button-Events.