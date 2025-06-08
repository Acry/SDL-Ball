// LevelManager.cpp
#include <SDL2/SDL_log.h>
#include <regex>
#include <fstream>
#include <filesystem>

#include "config.h"
#include "BrickTypes.h"
#include "LevelManager.h"
#include "BrickManager.h"
#include "PowerupManager.h"

#define DEBUG_BRICK_COUNT 0

BrickType charToBrickType(const char c) {
    switch (c) {
        case '1': return BrickType::Blue;
        case '2': return BrickType::Yellow;
        case '3': return BrickType::Cement;
        case '4': return BrickType::Glass;
        case '5': return BrickType::Green;
        case '6': return BrickType::Grey;
        case '7': return BrickType::Purple;
        case '8': return BrickType::White;
        case '9': return BrickType::Invisible;
        case 'A': return BrickType::Red;
        case 'B': return BrickType::Explosive;
        case 'C': return BrickType::Doom;
        case 'D': return BrickType::Base;
        default: return BrickType::None;
    }
}

PowerupType charToPowerUpType(const char c) {
    switch (c) {
        case '1': return PowerupType::GrowPaddle;
        case '2': return PowerupType::ShrinkPaddle;
        case '3': return PowerupType::Die;
        case '4': return PowerupType::Glue;
        case '5': return PowerupType::Multiball;
        case '6': return PowerupType::GoThrough;
        case '7': return PowerupType::Drop;
        case '8': return PowerupType::Detonate;
        case '9': return PowerupType::ExplosiveGrow;
        case 'A': return PowerupType::EasyBrick;
        case 'B': return PowerupType::Explosive;
        case 'C': return PowerupType::NextLevel;
        case 'D': return PowerupType::AimHelp;
        case 'E': return PowerupType::Coin;
        case 'F': return PowerupType::BigBall;
        case 'G': return PowerupType::NormalBall;
        case 'H': return PowerupType::SmallBall;
        case 'I': return PowerupType::Aim;
        case 'O': return PowerupType::Life;
        case 'P': return PowerupType::Gun;
        case 'R': return PowerupType::Laser;

        // Spezielle Random-Powerups
        case 'J': return PowerupType::Random10; // 10% Chance
        case 'K': return PowerupType::Random100; // 100% Chance
        case 'L': return PowerupType::Random5; // 5% Chance
        case 'M': return PowerupType::Random2; // 2% Chance
        case 'N': return PowerupType::Random1; // 1% Chance
        case 'Q': return PowerupType::RandomEvil; // 100% böses PowerUp

        case '0':
        default: return PowerupType::None;
    }
}

LevelManager::LevelManager(IEventManager *evtMgr) : eventManager(evtMgr) {
    if (eventManager) {
        eventManager->addListener(
            GameEvent::LevelThemeRequested,
            [this](const ThemeData &data) { onLevelThemeRequested(data); },
            this
        );
        eventManager->addListener(
            GameEvent::LevelRequested,
            [this](const LevelRequestedData &data) { onLevelRequested(data); },
            this
        );
    }
}

bool LevelManager::readLevelStructure() {
    const std::string fullPath = currentTheme;
    std::ifstream infile(fullPath);

    if (!infile.is_open()) {
        SDL_Log("Fehler: could not open file: %s", fullPath.c_str());
        return false;
    }

    if (infile.peek() == std::ifstream::traits_type::eof()) {
        SDL_Log("Error: file is empty: %s", fullPath.c_str());
        return false;
    }

    levelRanges.clear();
    std::string line;
    int level_count = 0;
    int current_row = 0;
    std::streampos start_pos;
    bool in_level = false;

    // todo, be permissive with whitespace instead of zero
    while (std::getline(infile, line)) {
        // trim Whitespace lineend
        line = std::regex_replace(line, std::regex("\\s+$"), "");

        if (line == "** Start **") {
            if (in_level) {
                SDL_Log("error: nested '** Start **' in level %d", level_count + 1);
                return false;
            }
            in_level = true;
            start_pos = infile.tellg();
            current_row = 0;
            continue;
        }

        if (line == "** Stop **") {
            if (!in_level) {
                SDL_Log("Error: '** Stop **' without '** Start **'");
                return false;
            }

            // Speichere die Position VOR der "** Stop **" Zeile
            std::streampos end_pos = infile.tellg() - static_cast<std::streamoff>(line.length() + 1);
            levelRanges.emplace_back(start_pos, end_pos);
            in_level = false;
            level_count++;
            continue;
        }

        // todo: Prüfen auf Steuerungskommandos
        // todo: Prüfen Typ 'D' dessen RGB-Farbwerte
        if (in_level && !line.empty() && !line.starts_with(">")) {
            // Prüfe Zeichengültigkeit
            static const std::string valid_chars = "0123456789ABCDEFGHIJKLMNOPQR";
            for (size_t i = 0; i < line.length(); i++) {
                if (valid_chars.find(line[i]) == std::string::npos) {
                    SDL_Log("Error: invalid char '%c' in level %d, line %d, col %ld", line[i],
                            level_count + 1, current_row + 1, i + 1);
                    return false;
                }
            }
            current_row++;
        }
    }
    if (in_level) {
        SDL_Log("Error: Missing '** Stop **' am Dateiende: %s", fullPath.c_str());
        return false;
    }
    if (levelRanges.empty()) {
        SDL_Log("No levels found: %s", fullPath.c_str());
        return false;
    }
    levelCount = levelRanges.size();
    SDL_Log("Level-structure %s parsed. count: %d", fullPath.c_str(), level_count);

    return true;
}

bool LevelManager::loadLevel(size_t level) {
    if (level == 0) return false; // No Level 0 for users (Hud)

    size_t internalLevel = level - 1;
    if (internalLevel >= levelRanges.size()) return false;

    std::ifstream infile(currentTheme);
    if (!infile.is_open()) return false;

    const auto &[startPos, endPos] = levelRanges[internalLevel];
    infile.seekg(startPos);

    currentLevelData = LevelData{
        .themeName = currentTheme,
        .bricks = {},
        .powerupData = {},
        .isDropping = false,
        .dropSpeed = 0,
        .currentLevel = internalLevel
    };
    std::string line;
    size_t row = 0;
    while (infile.tellg() < endPos && std::getline(infile, line)) {
        if (line.starts_with("> down")) {
            currentLevelData.isDropping = true;
            char *end;
            if (const long dropSpeed = std::strtol(line.substr(7).c_str(), &end, 10); *end != '\0' || dropSpeed <= 0) {
                SDL_Log("Ungültige Drop-Geschwindigkeit: %s", line.substr(7).c_str());
                currentLevelData.dropSpeed = 1000; // Fallback, bricks drop down once every 1 second.
            } else {
                currentLevelData.dropSpeed = static_cast<int>(dropSpeed);
            }
            continue;
        }

        size_t ch = 0;
#if DEBUG_BRICK_COUNT
        size_t brick_count_in_line = 0;
#endif
        while (ch < line.length()) {
            char powerupChar = line[ch];
            char type = line[ch + 1];

            if (type == '0') {
                ch += 2;
                continue;
            }
#if DEBUG_BRICK_COUNT
            brick_count_in_line++;
#endif
            BrickInfo info;
            size_t col = ch / 2; // Position basiert auf Zeichenpaaren
            info.x = PLAYFIELD_LEFT_BORDER + static_cast<float>(col) * BRICK_WIDTH;
            info.y = BRICKS_TOP_BORDER - static_cast<float>(row) * BRICK_HEIGHT;

            if (type == 'D' && ch + 7 < line.length()) {
                std::string rgb = line.substr(ch + 2, 6);
                int r = std::stoi(rgb.substr(0, 2), nullptr, 16);
                int g = std::stoi(rgb.substr(2, 2), nullptr, 16);
                int b = std::stoi(rgb.substr(4, 2), nullptr, 16);
                info.CustomBrickColor = {
                    static_cast<float>(r) / 255.0f,
                    static_cast<float>(g) / 255.0f,
                    static_cast<float>(b) / 255.0f,
                    1.0f
                };
                ch += 8; // PowerupType + D + RGB
            } else {
                ch += 2; // PowerupType + BrickType
            }

            info.type = charToBrickType(type);
            currentLevelData.bricks.push_back(info);
            if (PowerupType powerupType = charToPowerUpType(powerupChar);
                powerupType != PowerupType::None) {
                currentLevelData.powerupData.powerupMap[currentLevelData.bricks.size()] = powerupType;
            }
        }
#if DEBUG_BRICK_COUNT
            SDL_Log("Zeile %zu: %zu Bricks", row, brick_count_in_line);
#endif
        ++row;
    }
#if DEBUG_BRICK_COUNT
    SDL_Log("Level %zu enthält %zu Bricks", level, currentLevelData.bricks.size());
#endif
    eventManager->emit(GameEvent::LevelLoaded, currentLevelData);
    return true;
}

void LevelManager::onLevelRequested(const LevelRequestedData &data) {
    const size_t requestedLevel = data.level;
    if (loadLevel(requestedLevel)) {
        eventManager->emit(GameEvent::LevelLoaded, currentLevelData);
    } else {
        SDL_Log("Fehler beim Laden von Level %zu", requestedLevel);
    }
}

void LevelManager::onLevelThemeRequested(const ThemeData &data) {
    const SubThemeData requestedTheme = data.levelsTheme;
    if (setTheme(requestedTheme.subThemePath)) {
        LevelThemeData oData;
        oData.maxLevel = levelCount;
        eventManager->emit(GameEvent::LevelThemeChanged, oData);
    } else {
        SDL_Log("Fehler beim Laden des Level-Themes: %s", requestedTheme.subThemePath.c_str());
    }
}

bool LevelManager::setTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    clearTheme();
    currentTheme = themeName + "/" + "levels.txt";

    if (!readLevelStructure()) {
        SDL_Log("Error getting level theme: %s", currentTheme.c_str());
        clearTheme();
        return false;
    }
    return true;
}

void LevelManager::clearTheme() {
    levelRanges.clear();
    currentTheme.clear();
    levelCount = 0;
    currentLevelData = LevelData{};
}
