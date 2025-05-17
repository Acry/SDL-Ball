#include "SpriteSheetAnimation.h"
#include "game_state.h"

class powerupLoaderClass {
public:
    //most common
    string chances[4];
    int powerups;
    int powerupsGiven;
    string evilPowerups;

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

void load_level(const string& file, brick bricks[], const int level) {
    ifstream levelfile(file.data());
    if (!levelfile.is_open()) {
        SDL_Log(" Could not open%s", file.c_str());
        var.quit = true;
        return;
    }
    string line;
    int levelread = 0, brick = 0, ch = 0;
    var.numlevels = 0;
    var.scrollInfo.drop = false;

    while (!levelfile.eof()) {
        getline(levelfile, line);
        // Har vi fundet start?
        if (levelread == 0) {
            // Nej, er det nu?
            if (line.substr(0, 11) == "** Start **") {
                levelread++;
            }
        } else if (levelread == 1) {
            // Do the level stop now?
            if (line.substr(0, 10) == "** Stop **") {
                levelread = 0;
                var.numlevels++;
                brick = 0;
            } else {
                // Reading data from level
                if (var.numlevels == level) {
                    if (line[0] == '>') {
                        if (line.substr(0, 6) == "> down") {
                            var.scrollInfo.dropspeed = atol(line.substr(7, line.length()).data());
                            var.scrollInfo.drop = true;
                        }
                    } else {
                        while (line[ch] != 0) {
                            bricks[brick].powerup = line[ch];
                            bricks[brick].type = line[ch + 1];

                            if (bricks[brick].type == 'D') {
                                char rgb[3][5];

                                sprintf(rgb[0], "0x%c%c", line[ch + 2], line[ch + 3]);
                                sprintf(rgb[1], "0x%c%c", line[ch + 4], line[ch + 5]);
                                sprintf(rgb[2], "0x%c%c", line[ch + 6], line[ch + 7]);

                                bricks[brick].texture.textureProperties.glTexColorInfo[0] = 0.003921569 * strtol(rgb[0], nullptr, 16);
                                bricks[brick].texture.textureProperties.glTexColorInfo[1] = 0.003921569 * strtol(rgb[1], nullptr, 16);
                                bricks[brick].texture.textureProperties.glTexColorInfo[2] = 0.003921569 * strtol(rgb[2], nullptr, 16);

                                bricks[brick].texture.textureProperties.glParColorInfo[0] = 0.003921569 * strtol(rgb[0], nullptr, 16);
                                bricks[brick].texture.textureProperties.glParColorInfo[1] = 0.003921569 * strtol(rgb[1], nullptr, 16);
                                bricks[brick].texture.textureProperties.glParColorInfo[2] = 0.003921569 * strtol(rgb[2], nullptr, 16);

                                bricks[brick].texture.textureProperties.glTexColorInfo[3] = 1.0;
                                ch += 6;
                            }
                            // SDL_Log("Level:%s brick:%s Powerup:%s Type:%s\n", levelnum, brick, line[ch], line[ch+1]);

                            brick++;
                            ch += 2;
                        }
                        ch = 0;
                    } // Not a command
                } // denne level settes op
            }
        }
    }
    // SDL_Log("Read %d levels from '%s'", var.numlevels, file.c_str());
    levelfile.close();
}

void init_levels(brick bricks[], SpriteSheetAnimation texLvl[]) {
    powerupLoaderClass powerupLoader;

    //Temp storage for custom colors
    GLfloat tempCol[4], tempParCol[3];

    //Set dem op
    int i = 0;

    for (int row = 0; row < 23; row++) {
        for (int column = 0; column < 26; column++) {

            // Verfügbarer Bereich: 2.0 (-1 bis +1)
            // float total_width = 26 * BRICK_WIDTH;  // Gesamtbreite aller Steine
            // float total_height = 23 * BRICK_HEIGHT; // Gesamthöhe aller Steine

            // Abstand zwischen den Steinen
            // float x_spacing = (2.0f - total_width) / 27.0f;  // 27 Zwischenräume (inkl. Ränder)
            // float y_spacing = (2.0f - total_height) / 24.0f; // 24 Zwischenräume (inkl. Ränder)

            // Von -1 bis +1 in NDC, direkt Stein an Stein
            // glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection, flipping bottom and top for SDL2
            //         +1
            //         ^
            //         |
            // -1 <----+----> +1
            //         |
            //        -1
            bricks[i].pos_x = -1.0f + PILLAR_WIDTH + static_cast<float>(column) * BRICK_WIDTH;
            bricks[i].pos_y = -1.0f + 0.38f + static_cast<float>(row) * BRICK_HEIGHT;

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

                bricks[i].bricknum = column;

                bricks[i].row = row;
                updated_nbrick[row][column] = i; // This brick is active
                //SDL_Log("Brick:%d", nbrick[row][brick]);
            } else {
                bricks[i].score = 0;
                bricks[i].destroytowin = false;
                bricks[i].active = false;
                updated_nbrick[row][column] = -1;
                // SDL_Log("Brick:%d", nbrick[row][brick]);
            }

            if (bricks[i].type == '1') {
                bricks[i].texture = texLvl[6];
                bricks[i].opacity = texLvl[6].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '2') {
                bricks[i].texture = texLvl[7];
                bricks[i].opacity = texLvl[7].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '3') {
                bricks[i].powerup = '0';
                bricks[i].texture = texLvl[2];
                bricks[i].opacity = texLvl[2].textureProperties.glTexColorInfo[3];
                bricks[i].score = 30;
                bricks[i].destroytowin = false;
            } else if (bricks[i].type == '4') {
                bricks[i].texture = texLvl[4]; //glass texture
                bricks[i].hitsLeft = 2; //takes two hits to kill
                bricks[i].opacity = texLvl[4].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '5') {
                bricks[i].texture = texLvl[8];
                bricks[i].opacity = texLvl[8].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '6') {
                bricks[i].texture = texLvl[9];
                bricks[i].opacity = texLvl[9].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '7') {
                bricks[i].texture = texLvl[10];
                bricks[i].opacity = texLvl[10].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '8') {
                bricks[i].texture = texLvl[11];
                bricks[i].opacity = texLvl[11].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == '9') {
                bricks[i].texture = texLvl[5]; //invisible texture
                bricks[i].texture.frame = 1;
                bricks[i].hitsLeft = 3; //takes 3 to kill
                bricks[i].opacity = texLvl[5].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == 'A') {
                bricks[i].texture = texLvl[12];
                bricks[i].opacity = texLvl[12].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == 'B') {
                bricks[i].texture = texLvl[0];
                bricks[i].opacity = texLvl[0].textureProperties.glTexColorInfo[3];
            } else if (bricks[i].type == 'C') {
                bricks[i].texture = texLvl[3];
                bricks[i].opacity = texLvl[3].textureProperties.glTexColorInfo[3];
                bricks[i].powerup = powerupLoader.randomEvilPowerup();
            } else if (bricks[i].type == 'D') //type D get colors applied by loadlevel.
            {
                //Hold the colors while applying texture props
                memcpy(tempCol, bricks[i].texture.textureProperties.glTexColorInfo, sizeof(tempCol));
                memcpy(tempParCol, bricks[i].texture.textureProperties.glParColorInfo, sizeof(tempParCol));

                bricks[i].texture = texLvl[1];

                //Put pack the colors
                memcpy(bricks[i].texture.textureProperties.glTexColorInfo, tempCol, sizeof(tempCol));
                memcpy(bricks[i].texture.textureProperties.glParColorInfo, tempParCol, sizeof(tempParCol));

                bricks[i].opacity = tempCol[3];
            }

            i++;
        }
    }
    //    SDL_Log("Powerups given to this level:%d", powerupLoader.powerupsGiven);
}
