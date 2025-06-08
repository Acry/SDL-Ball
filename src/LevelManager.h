// LevelManager.h
#pragma once

#include <string>
#include <vector>

#include "IEventManager.h"

struct LevelOffset {
    std::streampos startPos;
    std::streampos endPos;
};

class LevelManager {
    IEventManager *eventManager{nullptr};
    std::vector<LevelOffset> levelRanges{};
    std::string currentTheme;
    size_t levelCount = 0;
    LevelData currentLevelData{};

public:
    explicit LevelManager(IEventManager *evtMgr = nullptr);

    ~LevelManager() = default;

    void clearTheme();

    bool setTheme(const std::string &themeName);

    [[nodiscard]] std::string getCurrentTheme() const { return currentTheme; };

    bool readLevelStructure();

    [[nodiscard]] size_t getLevelCount() const { return levelCount; }

    bool loadLevel(size_t level);

    void onLevelRequested(const LevelRequestedData &data);

    void onLevelThemeRequested(const ThemeData &data);
};
