#pragma once
#include "Texture.h"
#include <SDL2/SDL.h>
#include <epoxy/gl.h>
#include "ThemeManager.h"

struct score {
    int points;
    std::string level;
    std::string name;
};

class Menu {
    std::string saveGameName[6];
    int saveGameSlot;
    Texture tex[5];
    GLuint dl; joystickAttached;

    Menu();
    void refreshHighScoreList();
    void enterSaveGameName(const SDL_Event &e);
    void doMenu();
};