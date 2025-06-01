// LevelManager.h
#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <SDL_stdinc.h>

#include "Brick.h"


// LevelData-Struktur für allgemeine Level-Informationen
struct LevelData {
    bool isDropping;       // Ob das Level ein "dropping level" ist
    int dropSpeed;          // Geschwindigkeit des Drops (wenn hasDropping true ist)
    size_t brickCount;      // Anzahl der Bricks im Level
    std::string levelName;  // Optional: Name des Levels
};

struct BrickData {
    std::vector<Brick> bricks{};  // Alle Bricks des Levels
    LevelData levelInfo;        // Allgemeine Level-Informationen
};

// PowerupData-Struktur für den PowerupManager
struct PowerupData {
    struct PowerupInfo {
        size_t brickIndex;      // Index des Bricks in der Brick-Liste
        char type;              // Powerup-Typ (Buchstabencode)
        float posX, posY;       // Position im Level für stabilere Referenzierung
        bool active{false};     // Ist das Powerup aktiv/sichtbar?
    };

    std::vector<PowerupInfo> powerups{};
};

// Level-Offset-Typ für Start- und Endpositionen in der Levels-Datei
using LevelOffset = std::tuple<std::streampos, std::streampos>;

class LevelManager {
    EventManager* eventManager{nullptr};
    std::vector<LevelOffset> levelRanges{};
    std::string currentTheme;
    size_t levelCount = 0;

public:
    explicit LevelManager(EventManager* evtMgr = nullptr);

    ~LevelManager()= default;

    // Theme-Management
    void clearTheme();

    bool setTheme(const std::string &themeName);

    // Gibt den aktuellen Theme-Namen zurück
    [[nodiscard]] std::string getCurrentTheme() const{return currentTheme;};

    // Liest die Struktur der Levels-Datei und speichert Level-Bereiche
    bool readLevelsStructure();

    // Lädt alle Level und bereitet sie für das Spiel vor
    bool loadLevels();

    // Gibt die Anzahl der verfügbaren Level zurück
    [[nodiscard]] size_t getLevelCount() const { return levelCount; }

    BrickData getBrickDataForLevel(size_t level);
    PowerupData getPowerupDataForLevel(size_t level){ return {};};
};
