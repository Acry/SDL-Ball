// BackgroundManager.h
#pragma once

#include <filesystem>

#include "SpriteSheetAnimation.h"
#include "colors.h"
#include "IEventManager.h"

class TextureManager;

class BackgroundManager {
    std::string currentTheme;
    size_t maxLevel;
    GLuint backgroundDisplayList;
    // 4 colors for the background, will be set randomly
    // für einen Farbverlauf
    // Das Blending sollte bedingt nötig sein.
    GLfloat r[4]{}, g[4]{}, b[4]{}, a;
    const TextureManager *textureManager; // Referenz auf den TextureManager
    IEventManager *eventManager{nullptr};
    SpriteSheetAnimation texture; // Textur für den Hintergrund

    void drawQuad();

public:
    explicit BackgroundManager(const TextureManager &texMgr);

    ~BackgroundManager();

    void registerEvents(IEventManager *eventManager);

    bool updateBgIfNeeded(const Uint32 level);

    void draw() const;
};
