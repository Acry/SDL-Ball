// LevelManager.cpp
#include <SDL2/SDL_log.h>
#include <regex>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include "config.h"
#include "LevelManager.h"
#include "EventManager.h"

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
        case 'D': return BrickType::Base; // Custom Color
        default: return BrickType::None; // Sicherer Default-Wert
    }
}

PowerUpType charToPowerUpType(const char c) {
    switch (c) {
        case '1': return PowerUpType::GrowPaddle;
        case '2': return PowerUpType::ShrinkPaddle;
        case '3': return PowerUpType::Die;
        case '4': return PowerUpType::Glue;
        case '5': return PowerUpType::Multiball;
        case '6': return PowerUpType::GoThrough;
        case '7': return PowerUpType::Drop;
        case '8': return PowerUpType::Detonate;
        case '9': return PowerUpType::ExplosiveGrow;
        case 'A': return PowerUpType::EasyBrick;
        case 'B': return PowerUpType::Explosive;
        case 'C': return PowerUpType::NextLevel;
        case 'D': return PowerUpType::AimHelp;
        case 'E': return PowerUpType::Coin;
        case 'F': return PowerUpType::BigBall;
        case 'G': return PowerUpType::NormalBall;
        case 'H': return PowerUpType::SmallBall;
        case 'I': return PowerUpType::Aim;
        case 'O': return PowerUpType::Life;
        case 'P': return PowerUpType::Gun;
        case 'R': return PowerUpType::Laser;

        // Spezielle Random-Powerups
        case 'J': return PowerUpType::Random10; // 10% Chance
        case 'K': return PowerUpType::Random100; // 100% Chance
        case 'L': return PowerUpType::Random5; // 5% Chance
        case 'M': return PowerUpType::Random2; // 2% Chance
        case 'N': return PowerUpType::Random1; // 1% Chance
        case 'Q': return PowerUpType::RandomEvil; // 100% böses PowerUp

        case '0':
        default: return PowerUpType::None;
    }
}

LevelManager::LevelManager(EventManager *evtMgr) : eventManager(evtMgr) {
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
    if (level == 0) return false; // Kein Level 0 für Nutzer

    size_t internalLevel = level - 1;
    if (internalLevel >= levelRanges.size()) return false;

    std::ifstream infile(currentTheme);
    if (!infile.is_open()) return false;


    auto [start_pos, end_pos] = levelRanges[internalLevel];
    infile.seekg(start_pos);

    powerupData = PowerupData{};
    brickData = BrickData{};
    std::string line;
    Uint32 currentId = 0;
    size_t row = 0;
    while (infile.tellg() < end_pos && std::getline(infile, line)) {
        if (line.starts_with("> down")) {
            brickData.isDropping = true;
            char *end;
            if (const long dropSpeed = std::strtol(line.substr(7).c_str(), &end, 10); *end != '\0' || dropSpeed <= 0) {
                SDL_Log("Ungültige Drop-Geschwindigkeit: %s", line.substr(7).c_str());
                brickData.dropSpeed = 1000; // Fallback, bricks drop down once every 1 second.
            } else {
                brickData.dropSpeed = static_cast<int>(dropSpeed);
            }
            continue;
        }

        // Verarbeite normale Level-Zeilen
        size_t ch = 0;

        size_t brick_count_in_line = 0;
        while (ch < line.length()) {
            char powerupChar = line[ch];
            char type = line[ch + 1];

            if (type == '0') {
                ch += 2;
                continue;
            }

            brick_count_in_line++;
            Brick brick(eventManager);
            size_t col = ch / 2; // Position basiert auf Zeichenpaaren
            brick.pos_x = PLAYFIELD_LEFT_BORDER + static_cast<float>(col) * BRICK_WIDTH;
            brick.pos_y = BRICKS_TOP_BORDER - static_cast<float>(row) * BRICK_HEIGHT;
            brick.width = BRICK_WIDTH;
            brick.height = BRICK_HEIGHT;
            brick.id = currentId++;
            if (PowerUpType powerupType = charToPowerUpType(powerupChar); powerupType != PowerUpType::None) {
                powerupData.powerupMap[brick.id] = powerupType;
            }
            if (type == 'D' && ch + 7 < line.length()) {
                std::string rgb = line.substr(ch + 2, 6);
                int r = std::stoi(rgb.substr(0, 2), nullptr, 16);
                int g = std::stoi(rgb.substr(2, 2), nullptr, 16);
                int b = std::stoi(rgb.substr(4, 2), nullptr, 16);
                brick.color = {
                    static_cast<float>(r) / 255.0f,
                    static_cast<float>(g) / 255.0f,
                    static_cast<float>(b) / 255.0f,
                    1.0f
                };
                ch += 8; // PowerupType + D + RGB
            } else {
                ch += 2; // PowerupType + BrickType
            }

            brick.type = charToBrickType(type);
            brickData.bricks.push_back(brick);
        }
#if DEBUG_BRICK_COUNT
            SDL_Log("Zeile %zu: %zu Bricks", row, brick_count_in_line);
#endif
        ++row;
    }
#if DEBUG_BRICK_COUNT
    SDL_Log("Level %zu enthält %zu Bricks", level, result.bricks.size());
#endif
    currentLevel = internalLevel;
    LevelEventData data;
    data.currentlevel = currentLevel;
    data.brickData = brickData;
    data.powerupData = powerupData;
    eventManager->emit(GameEvent::LevelChanged, data);
    return true;
}

bool LevelManager::setTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    clearTheme();
    currentTheme = currentTheme + "/" + "levels.txt";

    if (!readLevelStructure()) {
        SDL_Log("Error getting level theme: %s", currentTheme.c_str());
        clearTheme();
        return false;
    }
    LevelThemeData data; // FIXME andere Event-Datenstruktur verwenden LevelThemeChangedData
    data.maxLevel = levelCount;
    eventManager->emit(GameEvent::LevelThemeChanged, data);
    return true;
}

void LevelManager::clearTheme() {
    levelRanges.clear();
    currentTheme.clear();
    levelCount = 0;
    powerupData = PowerupData{};
    brickData = BrickData{};
}
