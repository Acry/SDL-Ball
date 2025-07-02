// CodeManager.h
#pragma once
#include "BackgroundManager.h"
#include "BrickManager.h"
#include "IEventManager.h"
#include "ConfigFileManager.h"
#include "DisplayManager.hpp"
#include "EffectFactory.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "LevelManager.h"
#include "MouseManager.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "TextManager.h"
#include "ThemeManager.h"

class CodeManager {
    std::unique_ptr<BackgroundManager> backgroundManager;
    std::unique_ptr<BrickManager> brickManager;
    std::unique_ptr<ConfigFileManager> configFileManager;
    std::unique_ptr<DisplayManager> displayManager;
    std::unique_ptr<EventDispatcher> eventDispatcher;
    std::unique_ptr<IEventManager> eventManager;
    std::unique_ptr<KeyboardManager> keyboardManager;
    std::unique_ptr<LevelManager> levelManager;
    std::unique_ptr<MouseManager> mouseManager;
    std::unique_ptr<SettingsManager> settingsManager;
    std::unique_ptr<SoundManager> soundManager;
    std::unique_ptr<TextManager> textManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<ThemeManager> themeManager;
    std::unique_ptr<SpriteSheetAnimationManager> animationManager;
    std::unique_ptr<EffectFactory> effectFactory;

public:
    CodeManager::CodeManager();

    ~CodeManager();

    bool init();

private:
    void onLevelThemeRequested(const ThemeData &data) const;

    void applyTheme(const std::string &themeName) const;

    void initEventListeners();
};
