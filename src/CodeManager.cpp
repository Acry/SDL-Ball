// CodeManager.cpp
#include "CodeManager.h"
#include "EventManager.h"

CodeManager::CodeManager() = default;

bool CodeManager::init() {
    eventManager = std::make_unique<EventManager>();
    configFileManager = std::make_unique<ConfigFileManager>("./");
    auto settings = configFileManager->getSettingsFile();
    settingsManager = std::make_unique<SettingsManager>(settings);
    themeManager = std::make_unique<ThemeManager>(*configFileManager);

    displayManager = std::make_unique<DisplayManager>(eventManager.get());
    eventDispatcher = std::make_unique<EventDispatcher>(eventManager.get());
    keyboardManager = std::make_unique<KeyboardManager>(eventManager.get());
    mouseManager = std::make_unique<MouseManager>(eventManager.get());
    soundManager = std::make_unique<SoundManager>();
    soundManager->loadMusic("../tests/themes/test/music/Pixel Breaker.mp3");
    return true;
}

void CodeManager::run() {
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    displayManager->init(
        settingsManager->getSettings().displayToUse,
        settingsManager->getSettings().res_x,
        settingsManager->getSettings().res_y,
        settingsManager->getSettings().fullscreen
    );
    SDL_SetWindowBordered(displayManager->sdlWindow, SDL_FALSE);
    SDL_SetWindowOpacity(displayManager->sdlWindow, 0.0f);
    textureManager = std::make_unique<TextureManager>();
    const auto tex = std::make_unique<texture>();
    textureManager->load("../src-gfx/Splash.png", *tex);
    splashScene = std::make_unique<SplashScreen>(&tex->textureProperties);
    soundManager->playMusic();
    SDL_SetWindowOpacity(displayManager->sdlWindow, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    splashScene->draw();
    SDL_GL_SwapWindow(displayManager->sdlWindow);
    textManager = std::make_unique<TextManager>(eventManager.get());
    // init more managers
    while (running) {
        running = eventDispatcher->processEvents();
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // sceneManager->update(deltaTime);
        //SDL_GL_SwapWindow(displayManager->sdlWindow);
    }
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

        eventManager->emit(GameEvent::ThemeChanged, newTheme);
    }
}

CodeManager::~CodeManager() {
    //eventManager->removeListener(GameEvent::LevelThemeRequested, this);
}
