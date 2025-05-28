// BackgroundManager.h
#pragma once

#include <filesystem>

#include "SpriteSheetAnimation.h"
#include "colors.h"

class TextureManager;

class BackgroundManager {
    GLuint backgroundDisplayList;
    // 4 colors for the background, will be set randomly
    // für einen Farbverlauf
    // Das Blending sollte bedingt nötig sein.
    GLfloat r[4], g[4], b[4], a;
    const TextureManager *textureMgr; // Referenz auf den TextureManager
    SpriteSheetAnimation tex; // Textur für den Hintergrund

    // wird für die Display-Liste benötigt
    void drawQuad() const;

public:
    explicit BackgroundManager(const TextureManager &texMgr);

    ~BackgroundManager();

    bool updateBgIfNeeded(const Uint32 level, const std::filesystem::path &pathName);

    void draw() const;
};
