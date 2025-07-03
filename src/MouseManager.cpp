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
    SDL_SetRelativeMouseMode(SDL_TRUE);
    registerEventListeners();
}

void MouseManager::registerEventListeners() {
    eventManager->addListener(GameEvent::MouseMoved,
                              [this](const MouseEventData &data) {
                                  this->handleMouseMovedEvent(data);
                              }, this);

    eventManager->addListener(GameEvent::ViewportResized,
                              [this](const ViewportEventData &data) {
                                  this->handleResizeViewportEvent(data);
                              }, this);

    eventManager->addListener(GameEvent::MouseButtonPressed,
                              [this](const MouseEventData &data) {
                                  this->handleMouseButtonPressedEvent(data);
                              }, this);

    eventManager->addListener(GameEvent::MouseButtonReleased,
                              [this](const MouseEventData &data) {
                                  this->handleMouseButtonReleasedEvent(data);
                              }, this);

    eventManager->addListener(GameEvent::MouseWheelScrolled,
                              [this](const MouseEventData &data) {
                                  this->handleMouseWheelScrolledEvent(data);
                              }, this);
}

MouseManager::~MouseManager() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    cleanup();
}

void MouseManager::cleanup() {
    eventManager->removeListener(GameEvent::MouseMoved, this);
    eventManager->removeListener(GameEvent::ViewportResized, this);
    eventManager->removeListener(GameEvent::MouseButtonPressed, this);
    eventManager->removeListener(GameEvent::MouseButtonReleased, this);
    eventManager->removeListener(GameEvent::MouseWheelScrolled, this);
}

void MouseManager::normalizedMouseCoordinates() {
    normalizedX = (mouseX - viewportX - viewportW / 2.0f) * (2.0f / viewportW);
    normalizedY = (mouseY - viewportY - viewportH / 2.0f) * -1 * (2.0f / viewportH);
}

void MouseManager::handleMouseMovedEvent(const MouseEventData &data) {
    mouseX = data.x;
    mouseY = data.y;
    normalizedMouseCoordinates();
    const MouseCoordinatesNormalizedEventData newData{normalizedX, normalizedY};
    eventManager->emit(GameEvent::MouseCoordinatesNormalized, newData);
}


void MouseManager::handleResizeViewportEvent(const ViewportEventData &data) {
    viewportX = static_cast<float>(data.viewportX);
    viewportY = static_cast<float>(data.viewportY);
    viewportW = static_cast<float>(data.viewportW);
    viewportH = static_cast<float>(data.viewportH);
    normalizedMouseCoordinates();
    const MouseCoordinatesNormalizedEventData newData{normalizedX, normalizedY};
    eventManager->emit(GameEvent::MouseCoordinatesNormalized, newData);
}

void MouseManager::handleMouseButtonPressedEvent(const MouseEventData &data) {
    mouseX = data.x;
    mouseY = data.y;
    normalizedMouseCoordinates();
    const MouseEventData newData{normalizedX, normalizedY, data.button};
    eventManager->emit(GameEvent::MouseButtonPressedNormalized, newData);
}

void MouseManager::handleMouseButtonReleasedEvent(const MouseEventData &data) {
    mouseX = data.x;
    mouseY = data.y;
    normalizedMouseCoordinates();
    const MouseEventData newData{normalizedX, normalizedY, data.button};
    eventManager->emit(GameEvent::MouseButtonReleasedNormalized, newData);
}

void MouseManager::handleMouseWheelScrolledEvent(const MouseEventData &data) {
    mouseX = data.x;
    mouseY = data.y;
    normalizedMouseCoordinates();
    const MouseEventData newData{normalizedX, normalizedY, data.button, data.wheelX, data.wheelY, data.wheelFlipped};
    eventManager->emit(GameEvent::MouseWheelScrolledNormalized, newData);
}
