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
            // config.h left and right border
            // PLAYFIELD_LEFT_BORDER PLAYFIELD_RIGHT_BORDER PLAYFIELD_TOP_BORDER
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
