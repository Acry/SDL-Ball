// TextureManager.h
#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "SpriteSheetAnimation.h"

class TextureManager {
    std::vector<SpriteSheetAnimation*> loadedTextures;
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
    bool load(const std::filesystem::path &pathName, SpriteSheetAnimation& tex) const;
    void readTexProps(const std::filesystem::path &pathName, SpriteSheetAnimation& tex) const;
};
