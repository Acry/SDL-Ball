#pragma once
#include "MovingObject.h"
#include <string>
#include "TtfLegacyGl.h"

class powerupDescriptionClass : public MovingObject {
public:
    powerupDescriptionClass();
    void draw();
    Texture *tex;
    TtfLegacyGl& text;  // Als Referenz
    std::string name;
    std::string description;
};