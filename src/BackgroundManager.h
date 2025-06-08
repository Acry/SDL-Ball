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
    bool showBackgroundOverlay;
    std::array<Color, 4> colors;
    const TextureManager *textureManager;
    IEventManager *eventManager{nullptr};
    SpriteSheetAnimation texture;

    void drawQuad() const;

public:
    explicit BackgroundManager(const TextureManager &texMgr);

    ~BackgroundManager();

    void registerEvents(IEventManager *eventManager);

    bool updateBgIfNeeded(const Uint32 level);

    void draw() const;

    [[nodiscard]] bool isBackgroundOverlayEnabled() const { return showBackgroundOverlay; }
    void setBackgroundOverlayEnabled(bool enabled) { showBackgroundOverlay = enabled; }
};
