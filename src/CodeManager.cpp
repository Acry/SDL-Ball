// CodeManager.cpp
#include "CodeManager.h"
#include "EventManager.h"

CodeManager::CodeManager() = default;

bool CodeManager::init() {
    eventManager = std::make_unique<EventManager>();
    // Init Soundmanager
    // init SplashScene: new logo, new music
    configFileManager = std::make_unique<ConfigFileManager>("");
    settingsManager = std::make_unique<SettingsManager>(*configFileManager);
    themeManager = std::make_unique<ThemeManager>(*configFileManager);
    displayManager = std::make_unique<DisplayManager>(eventManager.get());
    soundManager = std::make_unique<SoundManager>();
    textManager = std::make_unique<TextManager>(eventManager.get());

    const auto &settings = settingsManager->getSettings();
    applyTheme(settings.gfxTheme);

    initEventListeners();
    // emit requests
    return true;
}

void CodeManager::initEventListeners() {
    eventManager->addListener(
        GameEvent::LevelThemeRequested,
        [this](const ThemeData &data) { onLevelThemeRequested(data); },
        this
    );
}

void CodeManager::onLevelThemeRequested(const ThemeData &data) const {
    const auto &requestedTheme = data.levelsTheme.subThemeName;

    if (themeManager->themeExists(requestedTheme)) {
        applyTheme(requestedTheme);
    } else {
        applyTheme(ThemeManager::getDefaultTheme());
    }
}

void CodeManager::applyTheme(const std::string &themeName) const {
    if (themeManager->setCurrentTheme(themeName)) {
        ThemeData newTheme;
        newTheme.spritesTheme = {"gfx", themeManager->getThemeFilePath("gfx")};
        newTheme.fontsTheme = {"font", themeManager->getThemeFilePath("font")};
        newTheme.levelsTheme = {"levels", themeManager->getThemeFilePath("levels.txt")};
        newTheme.backgroundTheme = {"bg", themeManager->getThemeFilePath("gfx/bg")};
        newTheme.soundTheme = {"snd", themeManager->getThemeFilePath("snd")};

        eventManager->emit(GameEvent::LevelThemeChanged, newTheme);
    }
}

CodeManager::~CodeManager() {
    eventManager->removeListener(GameEvent::LevelThemeRequested, this);
}
