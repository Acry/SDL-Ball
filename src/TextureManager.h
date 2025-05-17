// TextureManager.h
#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "Texture.h"

class TextureManager {
    std::vector<Texture*> loadedTextures;
    int maxTexSize;
public:
    TextureManager() {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    }

    ~TextureManager() {
        // Alle geladenen Texturen aufräumen
        for(const auto tex : loadedTextures) {
            glDeleteTextures(1, &tex->textureProperties.texture);
            delete tex;
        }
    }
    bool load(const std::filesystem::path &pathName, Texture& tex) const;
    void readTexProps(const std::filesystem::path &pathName, Texture& tex) const;
};
