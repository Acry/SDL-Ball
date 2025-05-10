// TextureManager.h
#pragma once
#include <string>

#include "texture.h"


class TextureManager {
public:
    static bool load(const std::string& file, texture& tex);
    void readTexProps(std::string fileName, texture& tex);
};


