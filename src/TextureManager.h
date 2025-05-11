// TextureManager.h
#pragma once
#include <string>
#include <vector>

#include "Texture.h"


class TextureManager {
    std::vector<Texture*> loadedTextures;
    int maxTexSize;
public:
    TextureManager() {
        // Max Texture Size vom OpenGL System abfragen
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    }

    ~TextureManager() {
        // Alle geladenen Texturen aufräumen
        for(const auto tex : loadedTextures) {
            glDeleteTextures(1, &tex->textureProperties.texture);
            delete tex;
        }
    }
    bool load(const std::string& file, Texture& tex);
    void readTexProps(std::string fileName, Texture& tex);
};

