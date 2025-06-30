#include <SDL2/SDL_events.h>

#include "EventDispatcher.h"

bool EventDispatcher::processEvents() const {
    SDL_Event event;
    bool running = true;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            // Display events
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    WindowEventData data{event.window.data1, event.window.data2};
                    eventManager->emit(GameEvent::WindowResized, data);
                }
                break;

            // Mouse events
            case SDL_MOUSEMOTION: {
                MouseEventData data{static_cast<float>(event.motion.x), static_cast<float>(event.motion.y)};
                eventManager->emit(GameEvent::MouseMoved, data);
                break;
            }

            case SDL_MOUSEBUTTONDOWN: {
                MouseEventData data{
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y),
                    event.button.button
                };
                eventManager->emit(GameEvent::MouseButtonPressed, data);
                break;
            }

            case SDL_MOUSEBUTTONUP: {
                MouseEventData data{
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y),
                    event.button.button
                };
                eventManager->emit(GameEvent::MouseButtonReleased, data);
                break;
            }

            case SDL_MOUSEWHEEL: {
                MouseEventData data{};
                // Setzen der Mausposition (optional)
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                data.x = static_cast<float>(mouseX);
                data.y = static_cast<float>(mouseY);
                // Wheel-spezifische Daten
                data.wheelX = event.wheel.x;
                data.wheelY = event.wheel.y;
                data.wheelFlipped = (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED);
                eventManager->emit(GameEvent::MouseWheelScrolled, data);
                break;
            }

            // Keyboard events
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
