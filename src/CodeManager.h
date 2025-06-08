// CodeManager.h
#pragma once
#include "IEventManager.h"
#include "ConfigFileManager.h"
#include "SettingsManager.h"
#include "ThemeManager.h"

class CodeManager {
    IEventManager &eventManager;
    ConfigFileManager &configFileManager;
    SettingsManager &settingsManager;
    ThemeManager &themeManager;

public:
    CodeManager(IEventManager &em, ConfigFileManager &cfm,
                SettingsManager &sm, ThemeManager &tm);

    ~CodeManager();

    bool init();

private:
    void onLevelThemeRequested(const ThemeData &data);

    void applyTheme(const std::string &themeName);

    void initEventListeners();
};
