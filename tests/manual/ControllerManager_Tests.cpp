#include <SDL2/SDL.h>
#include <iostream>

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

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
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    SDL_Log("AXISMOTION: Achse %d Wert: %d", event.caxis.axis, event.caxis.value);
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    SDL_Log("BUTTONDOWN: Button %d", event.cbutton.button);
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    SDL_Log("BUTTONUP: Button %d", event.cbutton.button);
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    SDL_Log("DEVICEADDED: Index %d", event.cdevice.which);
                    break;
                case SDL_CONTROLLERDEVICEREMOVED:
                    SDL_Log("DEVICEREMOVED: ID %d", event.cdevice.which);
                    break;
                case SDL_CONTROLLERDEVICEREMAPPED:
                    SDL_Log("DEVICEREMAPPED: ID %d", event.cdevice.which);
                    break;
                case SDL_CONTROLLERTOUCHPADDOWN:
                    SDL_Log("TOUCHPADDOWN: Finger %d X: %f Y: %f", event.ctouchpad.finger, event.ctouchpad.x,
                            event.ctouchpad.y);
                    break;
                case SDL_CONTROLLERTOUCHPADMOTION:
                    SDL_Log("TOUCHPADMOTION: Finger %d X: %f Y: %f", event.ctouchpad.finger, event.ctouchpad.x,
                            event.ctouchpad.y);
                    break;
                case SDL_CONTROLLERTOUCHPADUP:
                    SDL_Log("TOUCHPADUP: Finger %d X: %f Y: %f", event.ctouchpad.finger, event.ctouchpad.x,
                            event.ctouchpad.y);
                    break;
                case SDL_CONTROLLERSENSORUPDATE:
                    SDL_Log("SENSORUPDATE: Sensor %d Data[0]: %f", event.csensor.sensor, event.csensor.data[0]);
                    break;
                default:
                    break;
            }
        }
    }

    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}
