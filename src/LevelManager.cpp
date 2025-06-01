// LevelManager.cpp
#include <SDL2/SDL_log.h>
#include <regex>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "config.h"
#include "LevelManager.h"

// ersetzt loadlevel
//bool LevelManager::getLevelData(size_t level, brick bricks[], const std::string &filename) {
//    if (level >= level_ranges.size()) {
//        SDL_Log("Fehler: Level %ld existiert nicht", level);
//        return false;
//    }
//
//    std::ifstream infile(filename);
//    if (!infile.is_open()) {
//        SDL_Log("Fehler: Konnte Level-Datei nicht öffnen: %s", filename.c_str());
//        return false;
//    }
//
//    auto [start_pos, end_pos] = level_ranges[level];
//    infile.seekg(start_pos);
//
//    std::string line;
//    int brick_index = 0;
//    size_t ch = 0;
//
//    while (infile.tellg() < end_pos && std::getline(infile, line)) {
//        // Prüfe auf Steuerungskommandos
//        if (line[0] == '>') {
//            if (line.substr(0, 6) == "> down") {
//                var.scrollInfo.dropspeed = std::atol(line.substr(7).c_str());
//                var.scrollInfo.drop = true;
//            }
//            continue;
//        }
//
//        // Verarbeite Brick-Daten
//        ch = 0;
//        while (ch < line.length()) {
//            bricks[brick_index].powerup = line[ch];
//            bricks[brick_index].type = line[ch + 1];
//
//            if (bricks[brick_index].type == 'D') {
//                char rgb[3][5];
//                sprintf(rgb[0], "0x%c%c", line[ch + 2], line[ch + 3]);
//                sprintf(rgb[1], "0x%c%c", line[ch + 4], line[ch + 5]);
//                sprintf(rgb[2], "0x%c%c", line[ch + 6], line[ch + 7]);
//
//                const float color_factor = 0.003921569f; // 1/255
//                for (int i = 0; i < 3; i++) {
//                    float color_value = color_factor * std::strtol(rgb[i], nullptr, 16);
//                    bricks[brick_index].tex.prop.glTexColorInfo[i] = color_value;
//                    bricks[brick_index].tex.prop.glParColorInfo[i] = color_value;
//                }
//                bricks[brick_index].tex.prop.glTexColorInfo[3] = 1.0f;
//                ch += 8;
//            } else {
//                ch += 2;
//            }
//            brick_index++;
//        }
//    }
//
//    return true;
//}

LevelManager::LevelManager(EventManager* evtMgr) : eventManager(evtMgr) {}

bool LevelManager::readLevelsStructure() {
    // Ließt Struktur, prüft bestimmte Dinge der Levels-Datei ein und speichert die Level-Bereiche
    const std::string fullPath = currentTheme;
    std::ifstream infile(fullPath);
    // Prüfen ob Datei geöffnet werden kann
    if (!infile.is_open()) {
        SDL_Log("Fehler: Konnte Level-Datei nicht öffnen: %s", fullPath.c_str());
        return false;
    }
    // Prüfen ob Datei leer ist
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
            if (constexpr int EXPECTED_ROWS = NUM_BRICKS_Y; current_row != EXPECTED_ROWS) {
                constexpr int EXPECTED_COLS = NUM_BRICKS_X * 2; // 2 Zeichen pro Stein
                SDL_Log("Fehler: Ungültige Zeilenlänge in Level %d, Zeile %d: %zu (erwartet: %d)",
                        level_count + 1, current_row + 1, line.length(), EXPECTED_COLS);
                return false;
            }

            levelRanges.emplace_back(start_pos, infile.tellg());
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
    while (infile.tellg() < end_pos && std::getline(infile, line) && row < NUM_BRICKS_Y) {
        size_t col = 0;
        for (size_t ch = 0; ch + 1 < line.length() && col < NUM_BRICKS_X; ch += 2, ++col) {
            char powerup = line[ch];
            char type = line[ch + 1];
            if (type == '0') continue; // kein Brick

            Brick brick(eventManager);
            brick.pos_x = PLAYFIELD_LEFT_BORDER + col * BRICK_WIDTH;
            brick.pos_y = BRICKS_TOP_BORDER - row * BRICK_HEIGHT;
            brick.width = BRICK_WIDTH;
            brick.height = BRICK_HEIGHT;
            // Hier ggf. weitere Brick-Attribute setzen (z.B. Typ, Powerup)
            result.bricks.push_back(brick);
        }
        ++row;
    }
    result.levelInfo.brickCount = result.bricks.size();
    // Weitere LevelInfo-Felder ggf. setzen
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
