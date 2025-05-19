#pragma once
#include "MovingObject.h"
#include <string>
#include "TextManager.h"

class powerupDescriptionClass : public MovingObject {
public:
    powerupDescriptionClass();
    void draw();
    SpriteSheetAnimation *tex;
    TextManager& text;  // Als Referenz
    std::string name;
    std::string description;
};