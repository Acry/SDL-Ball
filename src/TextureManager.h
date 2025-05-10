// TextureManager.h
#pragma once
#include <string>

#include "Texture.h"


class TextureManager {
public:
    static bool load(const std::string& file, Texture& tex);
    void readTexProps(std::string fileName, Texture& tex);
};


