// CodeManager.cpp
#include "CodeManager.h"

CodeManager::CodeManager(IEventManager &em, ConfigFileManager &cfm,
                         SettingsManager &sm, ThemeManager &tm)
    : eventManager(em)
      , configFileManager(cfm)
      , settingsManager(sm)
      , themeManager(tm) {
}

bool CodeManager::init() {
    initEventListeners();

    // Initial-Theme aus Settings laden
    const auto &settings = settingsManager.getSettings();
    applyTheme(settings.gfxTheme);

    return true;
}

void CodeManager::initEventListeners() {
    eventManager.addListener(
        GameEvent::LevelThemeRequested,
        [this](const ThemeData &data) { onLevelThemeRequested(data); },
        this
    );
}

void CodeManager::onLevelThemeRequested(const ThemeData &data) {
    const auto &requestedTheme = data.levelsTheme.subThemeName;

    if (themeManager.themeExists(requestedTheme)) {
        applyTheme(requestedTheme);
    } else {
        applyTheme(ThemeManager::getDefaultTheme());
    }
}

void CodeManager::applyTheme(const std::string &themeName) {
    if (themeManager.setCurrentTheme(themeName)) {
        ThemeData newTheme;
        newTheme.spritesTheme = {"gfx", themeManager.getThemeFilePath("gfx")};
        newTheme.fontsTheme = {"font", themeManager.getThemeFilePath("font")};
        newTheme.levelsTheme = {"levels", themeManager.getThemeFilePath("levels.txt")};
        newTheme.backgroundTheme = {"bg", themeManager.getThemeFilePath("gfx/bg")};
        newTheme.soundTheme = {"snd", themeManager.getThemeFilePath("snd")};

        eventManager.emit(GameEvent::LevelThemeChanged, newTheme);
    }
}

CodeManager::~CodeManager() {
    eventManager.removeListener(GameEvent::LevelThemeRequested, this);
}
