#pragma once
#include "MovingObject.h"
#include "texture.h"
#include <string>

// Jeder Index entspricht einer bestimmten Font-Konfiguration aus der Datei fonts.txt.
#define FONT_MENU 0
#define FONT_ANNOUNCE_GOOD 1
#define FONT_ANNOUNCE_BAD 2
#define FONT_HIGHSCORE 3
#define FONT_MENUHIGHSCORE 4
#define FONT_INTROHIGHSCORE 5
#define FONT_INTRODESCRIPTION 6
#define FONT_NUM 7

class powerupDescriptionClass : public MovingObject {
public:
    powerupDescriptionClass();
    void draw() const;

    texture* tex;
    std::string name;
    std::string description;
};