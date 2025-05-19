// GameStateManager.h
#pragma once
#include "BackgroundManager.h"
#include "BallManager.h"
#include "ConfigFileManager.h"
#include "Display.hpp"
#include "Paddle.h"
#include "Player.h"
#include "SaveGameManager.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "ThemeManager.h"
#include "TextManager.h"

class GameManager {
    ConfigFileManager& configManager;
    SettingsManager& settingsManager;
    ThemeManager& themeManager;
    TextureManager textureManager;

    // GameObjects
    std::vector<GameObject*> gameObjects;
    Paddle* paddle;
    // ...

public:
    GameManager(ConfigFileManager& cfg, SettingsManager& settings, ThemeManager& theme)
        : configManager(cfg), settingsManager(settings), themeManager(theme) {
        // Initialisierung
        applySettings();
    }

    void applySettings() {
        // Theme aus Settings anwenden
        const settings& s = settingsManager.getSettings();
        themeManager.setCurrentTheme(s.gfxTheme);

        // Bei mehreren Theme-Typen:
        // themeManager.setCurrentTheme(s.gfxTheme, ThemeType::GRAPHICS);
        // themeManager.setCurrentTheme(s.sndTheme, ThemeType::SOUND);
        // themeManager.setCurrentTheme(s.lvlTheme, ThemeType::LEVEL);
    }

    void initializeGameObjects() {
        // Paddle erstellen und Texturen laden
        paddle = new Paddle();
        loadPaddleResources();
        // ...
    }

    void loadPaddleResources() {
        // Ressourcenpfade vom ThemeManager holen
        std::string basePath = themeManager.getThemeFilePath("gfx/paddle/base.png");
        std::string propsPath = themeManager.getThemeFilePath("gfx/paddle/base.txt");

        // Texturen über TextureManager laden
        textureManager.load(basePath, paddle->texture);
        textureManager.readTexProps(propsPath, paddle->texture);

        // Layer-Texturen
        paddle->layerTex = new SpriteSheetAnimation[2];

        std::string gluePath = themeManager.getThemeFilePath("gfx/paddle/glue.png");
        std::string gluePropsPath = themeManager.getThemeFilePath("gfx/paddle/glue.txt");
        textureManager.load(gluePath, paddle->layerTex[0]);
        textureManager.readTexProps(gluePropsPath, paddle->layerTex[0]);

        std::string gunPath = themeManager.getThemeFilePath("gfx/powerup/gun.png");
        std::string gunPropsPath = themeManager.getThemeFilePath("gfx/powerup/gun.txt");
        textureManager.load(gunPath, paddle->layerTex[1]);
        textureManager.readTexProps(gunPropsPath, paddle->layerTex[1]);
    }
};