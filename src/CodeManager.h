// CodeManager.h
#pragma once
#include "BackgroundManager.h"
#include "IEventManager.h"
#include "ConfigFileManager.h"
#include "DisplayManager.hpp"
#include "EffectFactory.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "SplashScreen.h"
#include "TextManager.h"
#include "ThemeManager.h"

class CodeManager {
public:
    CodeManager();

    ~CodeManager();

    bool init();

    void run();

private:
    std::unique_ptr<IEventManager> eventManager;
    std::unique_ptr<DisplayManager> displayManager;
    std::unique_ptr<EventDispatcher> eventDispatcher;

    std::unique_ptr<ConfigFileManager> configFileManager;
    std::unique_ptr<ThemeManager> themeManager;
    std::unique_ptr<SettingsManager> settingsManager;
    std::unique_ptr<KeyboardManager> keyboardManager;
    std::unique_ptr<MouseManager> mouseManager;

    std::unique_ptr<SoundManager> soundManager;
    std::unique_ptr<TextManager> textManager;
    std::unique_ptr<TextureManager> textureManager;


    std::unique_ptr<SplashScreen> splashScene;
    std::unique_ptr<BackgroundManager> backgroundManager;

    bool running = true;

    void onLevelThemeRequested(const ThemeData &data) const;

    void applyTheme(const std::string &themeName) const;

    void initEventListeners();
};
