#include <map>
#include <regex>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
//#include <SDL2/SDL.h>

//#include "game_state.h"
#include "ThemeManager.h"
#include "settings_manager.h"

using LevelOffset = std::tuple<std::streampos, std::streampos>;
extern ThemeManager themeManager;
extern settings setting;

// level ranges in levels.txt
std::vector<LevelOffset> level_ranges;

class powerupLoaderClass {
public:
    //most common
    std::string chances[4];
    int powerups;
    int powerupsGiven;
    std::string evilPowerups;

    powerupLoaderClass() {
        int chance = -1;
        powerups = 0;
        powerupsGiven = 0;
        string line;

        // Which powerups are evil?
        evilPowerups = "2H37"; //P

        ifstream powerFile(themeManager.getThemeFilePath("/powerups.txt", setting.lvlTheme).data());
        if (!powerFile.is_open()) {
            SDL_Log(" Could not open 'powerups.txt'");
            var.quit = true;
            return;
        }

        while (!powerFile.eof()) {
            getline(powerFile, line);
            if (line.compare("Most:") == 0) {
                chance = 0;
            } else if (line.compare("More:") == 0) {
                chance = 1;
            } else if (line.compare("Less:") == 0) {
                chance = 2;
            } else if (line.compare("Least:") == 0) {
                chance = 3;
            } else {
                //No matches, either its a comment or a powerup.
                if (line.compare(0, 2, "//") != 0 && line.compare("") != 0) {
                    chances[chance] += line[0];
                    powerups++;
                }
            }
        }
    }

    char selectRandomPowerup() {
        int i = rand() % 100;
        powerupsGiven++;
        int group = (i < 1) ? 3 : (i < 11) ? 2 : (i < 41) ? 1 : 0;
        i = rand() % chances[group].length();
        return chances[group][i];
    }

    char randomPowerup(const char p) {
        const int i = rand() % 100;
        switch (p) {
            case 'Q': // 100% evil
                return randomEvilPowerup();
            case 'K': // 100%
                return selectRandomPowerup();
            case 'N': // 1%
                if (i < 1) return selectRandomPowerup();
                return '0';
            case 'M': // 2%
                if (i < 2) return selectRandomPowerup();
                return '0';
            case 'L': // 5%
                if (i < 5) return selectRandomPowerup();
                return '0';
            case 'J': // 10%
                if (i < 10) return selectRandomPowerup();
                return '0';
            default:
                return p;
        }
    }

    char randomEvilPowerup() const {
        const int i = rand() % evilPowerups.length();
        return evilPowerups[i];
    }
};

// ersetzt loadlevel
bool get_level_data(size_t level, brick bricks[], const std::string &filename) {
    if (level >= level_ranges.size()) {
        SDL_Log("Fehler: Level %ld existiert nicht", level);
        return false;
    }

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        SDL_Log("Fehler: Konnte Level-Datei nicht öffnen: %s", filename.c_str());
        return false;
    }

    auto [start_pos, end_pos] = level_ranges[level];
    infile.seekg(start_pos);

    std::string line;
    int brick_index = 0;
    size_t ch = 0;

    while (infile.tellg() < end_pos && std::getline(infile, line)) {
        // Prüfe auf Steuerungskommandos
        if (line[0] == '>') {
            if (line.substr(0, 6) == "> down") {
                var.scrollInfo.dropspeed = std::atol(line.substr(7).c_str());
                var.scrollInfo.drop = true;
            }
            continue;
        }

        // Verarbeite Brick-Daten
        ch = 0;
        while (ch < line.length()) {
            bricks[brick_index].powerup = line[ch];
            bricks[brick_index].type = line[ch + 1];

            if (bricks[brick_index].type == 'D') {
                char rgb[3][5];
                sprintf(rgb[0], "0x%c%c", line[ch + 2], line[ch + 3]);
                sprintf(rgb[1], "0x%c%c", line[ch + 4], line[ch + 5]);
                sprintf(rgb[2], "0x%c%c", line[ch + 6], line[ch + 7]);

                const float color_factor = 0.003921569f; // 1/255
                for (int i = 0; i < 3; i++) {
                    float color_value = color_factor * std::strtol(rgb[i], nullptr, 16);
                    bricks[brick_index].tex.prop.glTexColorInfo[i] = color_value;
                    bricks[brick_index].tex.prop.glParColorInfo[i] = color_value;
                }
                bricks[brick_index].tex.prop.glTexColorInfo[3] = 1.0f;
                ch += 8;
            } else {
                ch += 2;
            }
            brick_index++;
        }
    }

    return true;
}

bool read_levels_structure(const std::string &filename) {
    std::ifstream infile(filename);

    // Prüfen ob Datei geöffnet werden kann
    if (!infile.is_open()) {
        SDL_Log("Fehler: Konnte Level-Datei nicht öffnen: %s", filename.c_str());
        return false;
    }
    // Prüfen ob Datei leer ist
    if (infile.peek() == std::ifstream::traits_type::eof()) {
        SDL_Log("Level-Datei ist leer: %s", filename.c_str());
        return false;
    }

    level_ranges.clear();
    std::string line;
    int level_count = 0;
    constexpr int EXPECTED_ROWS = 23;
    int current_row = 0;
    std::streampos start_pos;
    bool in_level = false;

    // todo, be permissive with whitespace instead of zero
    while (std::getline(infile, line)) {
        constexpr int EXPECTED_COLS = 52;

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
            if (current_row != EXPECTED_ROWS) {
                SDL_Log("Fehler: Ungültige Zeilenlänge in Level %d, Zeile %d: %zu (erwartet: %d)",
                        level_count + 1, current_row + 1, line.length(), EXPECTED_COLS);
                return false;
            }

            level_ranges.push_back({start_pos, infile.tellg()});
            in_level = false;
            level_count++;
            continue;
        }

        // todo: Prüfen auf Steuerungskommandos
        // todo: Prüfen Typ 'D' dessen RGB-Farbwerte
        if (in_level && line.length() > 0 && !line.starts_with(">")) {
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
        SDL_Log("Fehler: Fehlendes '** Stop **' am Dateiende: %s", filename.c_str());
        return false;
    }
    // Prüfen ob Level gefunden wurden
    if (level_ranges.empty()) {
        SDL_Log("Keine Level in Datei gefunden: %s", filename.c_str());
        return false;
    }
    var.numlevels = level_count;
    SDL_Log("Level-Struktur aus %s gelesen. %d Level erkannt", filename.c_str(), level_count);
    return true;
}

// if (gVar.newLevel) {
bool set_up_bricks_for_level(const size_t level, brick bricks[], texture texLvl[]) {
    if (level >= level_ranges.size()) {
        SDL_Log("Fehler: Level %ld existiert nicht", level);
        return false;
    }
    if (const bool have_data = get_level_data(level, bricks,
                                              themeManager.getThemeFilePath("levels.txt", setting.lvlTheme)); !
        have_data) {
        SDL_Log("Fehler: Konnte Level-Daten nicht lesen");
        return false;
    }
    powerupLoaderClass powerupLoader;
    //Temp storage for custom colors
    GLfloat tempCol[4], tempParCol[3];

    // Set dem op
    int i = 0;

    for (int row = 0; row < 23; row++) {
        for (int brick = 0; brick < 26; brick++) {
            // todo: säulen links und rechts berücksichtigen
            bricks[i].posx = -1.0f + (brick + 0.5f) * BRICK_WIDTH;
            bricks[i].posy = 1.0f - (row + 0.5f) * BRICK_HEIGHT;

            if (bricks[i].type != '0') {
                bricks[i].active = true;
                bricks[i].collide = true;
                bricks[i].isdyingnormally = false;
                bricks[i].isexploding = false;
                bricks[i].reflect = true;

                bricks[i].fade = 1.0;
                bricks[i].fadespeed = 2.0;
                bricks[i].zoom = 1.0;
                bricks[i].zoomspeed = 4.0;
                bricks[i].width = BRICK_WIDTH;
                bricks[i].height = BRICK_HEIGHT;

                bricks[i].score = 11;
                bricks[i].destroytowin = true;
                bricks[i].hitsLeft = 1;

                bricks[i].powerup = powerupLoader.randomPowerup(bricks[i].powerup);

                bricks[i].bricknum = brick;

                bricks[i].row = row;
                updated_nbrick[row][brick] = i; // This brick is active
                //SDL_Log("Brick:%d", nbrick[row][brick]);
            } else {
                bricks[i].score = 0;
                bricks[i].destroytowin = false;
                bricks[i].active = false;
                updated_nbrick[row][brick] = -1;
                // SDL_Log("Brick:%d", nbrick[row][brick]);
            }

            if (bricks[i].type == '1') {
                bricks[i].tex = texLvl[6];
                bricks[i].opacity = texLvl[6].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '2') {
                bricks[i].tex = texLvl[7];
                bricks[i].opacity = texLvl[7].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '3') {
                bricks[i].powerup = '0';
                bricks[i].tex = texLvl[2];
                bricks[i].opacity = texLvl[2].prop.glTexColorInfo[3];
                bricks[i].score = 30;
                bricks[i].destroytowin = false;
            } else if (bricks[i].type == '4') {
                bricks[i].tex = texLvl[4]; //glass texture
                bricks[i].hitsLeft = 2; //takes two hits to kill
                bricks[i].opacity = texLvl[4].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '5') {
                bricks[i].tex = texLvl[8];
                bricks[i].opacity = texLvl[8].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '6') {
                bricks[i].tex = texLvl[9];
                bricks[i].opacity = texLvl[9].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '7') {
                bricks[i].tex = texLvl[10];
                bricks[i].opacity = texLvl[10].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '8') {
                bricks[i].tex = texLvl[11];
                bricks[i].opacity = texLvl[11].prop.glTexColorInfo[3];
            } else if (bricks[i].type == '9') {
                bricks[i].tex = texLvl[5]; //invisible texture
                bricks[i].tex.frame = 1;
                bricks[i].hitsLeft = 3; //takes 3 to kill
                bricks[i].opacity = texLvl[5].prop.glTexColorInfo[3];
            } else if (bricks[i].type == 'A') {
                bricks[i].tex = texLvl[12];
                bricks[i].opacity = texLvl[12].prop.glTexColorInfo[3];
            } else if (bricks[i].type == 'B') {
                bricks[i].tex = texLvl[0];
                bricks[i].opacity = texLvl[0].prop.glTexColorInfo[3];
            } else if (bricks[i].type == 'C') {
                bricks[i].tex = texLvl[3];
                bricks[i].opacity = texLvl[3].prop.glTexColorInfo[3];
                bricks[i].powerup = powerupLoader.randomEvilPowerup();
            } else if (bricks[i].type == 'D') //type D get colors applied by loadlevel.
            {
                //Hold the colors while applying texture props
                memcpy(tempCol, bricks[i].tex.prop.glTexColorInfo, sizeof(tempCol));
                memcpy(tempParCol, bricks[i].tex.prop.glParColorInfo, sizeof(tempParCol));

                bricks[i].tex = texLvl[1];

                //Put pack the colors
                memcpy(bricks[i].tex.prop.glTexColorInfo, tempCol, sizeof(tempCol));
                memcpy(bricks[i].tex.prop.glParColorInfo, tempParCol, sizeof(tempParCol));

                bricks[i].opacity = tempCol[3];
            }

            i++;
        }
    }
    return true;
    //    SDL_Log("Powerups given to this level:%d", powerupLoader.powerupsGiven);
}

bool load_levels() {
    const std::string levels_file_name = themeManager.getThemeFilePath("levels.txt", setting.lvlTheme);
    if (const bool have_structure = read_levels_structure(levels_file_name); !have_structure) {
        SDL_Log("Fehler: Konnte Level-Struktur nicht lesen");
        return false;
    }
    return true;
}
