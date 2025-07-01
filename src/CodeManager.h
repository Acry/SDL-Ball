// CodeManager.h
#pragma once
#include "IEventManager.h"
#include "ConfigFileManager.h"
#include "SettingsManager.h"
#include "ThemeManager.h"

class CodeManager {
    std::unique_ptr<IEventManager> eventManager;
    std::unique_ptr<ConfigFileManager> configFileManager;
    std::unique_ptr<SettingsManager> settingsManager;
    std::unique_ptr<ThemeManager> themeManager;

public:
    CodeManager::CodeManager();

    ~CodeManager();

    bool init();

private:
    void onLevelThemeRequested(const ThemeData &data) const;

    void applyTheme(const std::string &themeName) const;

    void initEventListeners();
};
