#include "EventDispatcher.h"
#include <SDL2/SDL_events.h>

bool EventDispatcher::processEvents() const {
    SDL_Event event;
    bool running = true;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    WindowEventData data{event.window.data1, event.window.data2};
                    eventManager->emit(GameEvent::WindowResized, data);
                }
                break;

            case SDL_MOUSEMOTION: {
                MouseEventData data{static_cast<float>(event.motion.x), static_cast<float>(event.motion.y)};
                eventManager->emit(GameEvent::MouseMoved, data);
                break;
            }

            case SDL_KEYDOWN: // Menu öffnen -> KeyboardManager -> emit(GameEvent::OpenenMenu);
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                eventManager->emit(GameEvent::KeyPressed, KeyboardEventData{event.key.keysym.sym, true});
                break;

            case SDL_KEYUP:
                eventManager->emit(GameEvent::KeyReleased, KeyboardEventData{event.key.keysym.sym, false});
                break;
            default: ;
        }
    }
    return running;
}
