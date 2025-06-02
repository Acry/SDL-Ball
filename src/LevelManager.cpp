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

LevelManager::LevelManager(EventManager *evtMgr) : eventManager(evtMgr) {
}

bool LevelManager::readLevelsStructure() {
    // Liest Struktur, prüft bestimmte Dinge der Levels-Datei ein und speichert die Level-Bereiche
    const std::string fullPath = currentTheme;
    std::ifstream infile(fullPath);

    if (!infile.is_open()) {
        SDL_Log("Fehler: Konnte Level-Datei nicht öffnen: %s", fullPath.c_str());
        return false;
    }

    if (infile.peek() == std::ifstream::traits_type::eof()) {
        SDL_Log("Level-Datei ist leer: %s", fullPath.c_str());
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
        // Entferne Whitespace am Ende
        line = std::regex_replace(line, std::regex("\\s+$"), "");

        if (line == "** Start **") {
            if (in_level) {
                SDL_Log("Fehler: Verschachteltes '** Start **' in Level %d", level_count + 1);
                return false;
            }
            in_level = true;
            start_pos = infile.tellg();
            current_row = 0;
            continue;
        }

        if (line == "** Stop **") {
            if (!in_level) {
                SDL_Log("Fehler: '** Stop **' ohne vorheriges '** Start **'");
                return false;
            }

            // Prüfe Zeilenanzahl
            //if (constexpr int EXPECTED_ROWS = NUM_BRICKS_Y; current_row != EXPECTED_ROWS) {
            //    constexpr int EXPECTED_COLS = NUM_BRICKS_X * 2; // 2 Zeichen pro Stein
            //    SDL_Log("Fehler: Ungültige Zeilenlänge in Level %d, Zeile %d: %zu (erwartet: %d)",
            //            level_count + 1, current_row + 1, line.length(), EXPECTED_COLS);
            //    return false;
            //}

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
            // Prüfe Zeilenlänge
            // if (line.length() != EXPECTED_COLS) {
            //     std::cerr << "Fehler: Ungültige Zeilenlänge in Level " << level_count + 1
            //              << ", Zeile " << current_row + 1 << ": " << line.length()
            //              << " (erwartet: " << EXPECTED_COLS << ")" << std::endl;
            //     return false;
            // }

            // Prüfe Zeichengültigkeit
            static const std::string valid_chars = "0123456789ABCDEFGHIJKLMNOPQR";
            for (size_t i = 0; i < line.length(); i++) {
                if (valid_chars.find(line[i]) == std::string::npos) {
                    SDL_Log("Fehler: Ungültiges Zeichen '%c' in Level %d, Zeile %d, Position %ld", line[i],
                            level_count + 1, current_row + 1, i + 1);
                    return false;
                }
            }
            current_row++;
        }
    }
    if (in_level) {
        SDL_Log("Fehler: Fehlendes '** Stop **' am Dateiende: %s", fullPath.c_str());
        return false;
    }
    // Prüfen ob Level gefunden wurden
    if (levelRanges.empty()) {
        SDL_Log("Keine Level in Datei gefunden: %s", fullPath.c_str());
        return false;
    }
    levelCount = levelRanges.size(); // ?
    SDL_Log("Level-Struktur aus %s gelesen. %d Level erkannt", fullPath.c_str(), level_count);

    LevelEventData data;
    data.currentlevel = currentLevel;
    data.maxLevel = levelCount;
    eventManager->emit(GameEvent::LevelThemeChanged, data);
    return true;
}

bool LevelManager::loadLevels() {
    currentTheme = currentTheme + "/" + "levels.txt";
    if (const bool have_structure = readLevelsStructure(); !have_structure) {
        SDL_Log("Fehler: Konnte Level-Struktur nicht lesen");
        return false;
    }

    return true;
}

BrickData LevelManager::getBrickDataForLevel(size_t level) {
    BrickData result;
    if (level == 0) return result; // Kein Level 0 für Nutzer

    size_t internalLevel = level - 1;
    if (internalLevel >= levelRanges.size()) return result;

    std::ifstream infile(currentTheme);
    if (!infile.is_open()) return result;

    auto [start_pos, end_pos] = levelRanges[internalLevel];
    infile.seekg(start_pos);

    std::string line;

    size_t row = 0;
    while (infile.tellg() < end_pos && std::getline(infile, line)) {
        // Prüfe auf Dropping-Level Kommando
        if (line.starts_with("> down")) {
            result.isDropping = true;
            result.dropSpeed = std::atoi(line.substr(7).c_str());
            continue;
        }

        // Verarbeite normale Level-Zeilen
        size_t ch = 0;

        size_t brick_count_in_line = 0;
        while (ch < line.length()) {
            char powerup = line[ch];
            char type = line[ch + 1];

            if (type == '0') {
                ch += 2;
                continue;
            }

            brick_count_in_line++;
            Brick brick(eventManager);
            size_t col = ch / 2; // Position basiert auf Zeichenpaaren
            brick.pos_x = PLAYFIELD_LEFT_BORDER + col * BRICK_WIDTH;
            brick.pos_y = BRICKS_TOP_BORDER - row * BRICK_HEIGHT;
            brick.width = BRICK_WIDTH;
            brick.height = BRICK_HEIGHT;
            //brick.powerupType = powerup;

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
            result.bricks.push_back(brick);
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
    data.maxLevel = levelCount;
    eventManager->emit(GameEvent::LevelChanged, data);
    return result;
}

bool LevelManager::setTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    clearTheme();
    currentTheme = themeName;

    if (!loadLevels()) {
        SDL_Log("Error getting font theme: %s", currentTheme.c_str());
        clearTheme();
        return false;
    }
    return true;
}

void LevelManager::clearTheme() {
    levelRanges.clear();
    currentTheme.clear();
    levelCount = 0;
}
