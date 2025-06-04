// LevelManager.h
#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <SDL_stdinc.h>

#include "Brick.h"
#include "Powerup.h"

struct BrickData {
    std::vector<Brick> bricks{}; // Alle Bricks des Levels
    bool isDropping{}; // Ob das Level ein "dropping level" ist
    int dropSpeed{}; // Geschwindigkeit des Drops (wenn hasDropping true ist)
};

struct PowerupData {
    std::map<Uint32, PowerUpType> powerupMap{}; // Map für Brick-ID -> PowerUp-Typ
};

using LevelOffset = std::tuple<std::streampos, std::streampos>;

class LevelManager {
    EventManager *eventManager{nullptr};
    std::vector<LevelOffset> levelRanges{};
    std::string currentTheme;
    size_t levelCount = 0;
    size_t currentLevel = 0; // Aktuelles Level, 0-basiert für interne Verwendung
    std::map<Uint32, PowerUpType> powerupMap{}; // Map für Brick-ID -> PowerUp-Typ
    PowerupData powerupData{}; // Member-Variable für PowerUp-Informationen
    BrickData brickData{};

public:
    explicit LevelManager(EventManager *evtMgr = nullptr);

    ~LevelManager() = default;

    void clearTheme();

    bool setTheme(const std::string &themeName);

    [[nodiscard]] std::string getCurrentTheme() const { return currentTheme; };

    bool readLevelStructure();

    [[nodiscard]] size_t getLevelCount() const { return levelCount; }

    bool loadLevel(size_t level);
};
