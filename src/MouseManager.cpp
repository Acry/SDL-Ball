// MouseManager.cpp
#include <SDL2/SDL.h>
#include "MouseManager.h"

// Center Mouse
// SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);

// SDL_SetWindowGrab(window, SDL_TRUE);

// Listen for GameEvent::MenuOpened and GameEvent::MenuClosed
// SDL_SetRelativeMouseMode(SDL_TRUE);
// SDL_SetRelativeMouseMode(SDL_FALSE);
MouseManager::MouseManager(IEventManager *evtMgr) : IInputManager(evtMgr) {
    eventManager->addListener(GameEvent::MouseMoved,
                              [this](const MouseEventData &data) {
                                  this->handleMouseEvent(data);
                              }, this);

    eventManager->addListener(GameEvent::ViewportResized,
                              [this](const ViewportEventData &data) {
                                  this->handleResizeViewportEvent(data);
                              }, this);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

MouseManager::~MouseManager() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    cleanup();
}

void MouseManager::cleanup() {
    eventManager->removeListener(GameEvent::MouseMoved, this);
    eventManager->removeListener(GameEvent::ViewportResized, this);
}

void MouseManager::normalizedMouseCoordinates() {
    normalizedX = (mouseX - viewportX - viewportW / 2.0f) * (2.0f / viewportW);
    normalizedY = (mouseY - viewportY - viewportH / 2.0f) * -1 * (2.0f / viewportH);
    const MouseCoordinatesNormalizedEventData data{normalizedX, normalizedY};
    eventManager->emit(GameEvent::MouseCoordinatesNormalized, data);
}

void MouseManager::handleMouseEvent(const MouseEventData &data) {
    mouseX = data.x;
    mouseY = data.y;
    normalizedMouseCoordinates();
}

void MouseManager::handleResizeViewportEvent(const ViewportEventData &data) {
    viewportX = static_cast<float>(data.viewportX);
    viewportY = static_cast<float>(data.viewportY);
    viewportW = static_cast<float>(data.viewportW);
    viewportH = static_cast<float>(data.viewportH);
    normalizedMouseCoordinates();
}
