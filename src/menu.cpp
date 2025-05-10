#include "texture.h"

extern SettingsManager settingsManager;
extern SaveFileManager saveManager;
extern player_struct SOLPlayer;

struct score {
    int points;
    string level;
    string name;
};

score *sortScores(int *rl) {
    ifstream hsList;
    int i = 0, t;

    score *final = nullptr, *temp = nullptr;
    final = new score[1];

    hsList.open(configFile.getHighScoreFile().data());

    if (hsList.is_open()) {
        int delim[2];
        string line;
        while (!hsList.eof()) {
            getline(hsList, line);

            if (!line.empty()) {
                if (line[0] == '[') {
                    delim[0] = line.find(']');
                    delim[1] = line.find('|', delim[0] + 1);

                    final[i].level = line.substr(0, delim[0] + 1);
                    final[i].level += "  ";
                    final[i].points = atoi(line.substr(delim[0] + 1, delim[1]).data());
                    final[i].name = line.substr(delim[1] + 1);
                } else {
                    final[i].level.clear();
                    final[i].points = atoi(line.substr(0, line.find('|')).data());
                    final[i].name = line.substr(line.find('|') + 1);
                }

                temp = new score[i + 1];

                for (t = 0; t < i + 1; t++) {
                    temp[t] = final[t];
                }

                delete[] final;
                final = new score[i + 2];

                for (t = 0; t < i + 1; t++) {
                    final[t] = temp[t];
                }
                delete[] temp;

                i++;
            }
        }
        hsList.close();
    }

    if (i > 20)
        *rl = 20;
    else
        *rl = i;

    bool done = false;
    bool swaps = false;

    temp = new score[1];
    while (!done) {
        for (int k = 0; k < i; k++) {
            for (t = 0; t < i; t++) {
                if (final[k].points > final[t].points) {
                    swaps = true;
                    temp[0] = final[t];
                    final[t] = final[k];
                    final[k] = temp[0];
                } else {
                    swaps = false;
                }
            }
        }
        if (swaps == 0) {
            done = true;
        }
    }
    delete[] temp;
    return final;
}

class menuClass {
    string saveGameName[6]; //The name of saveGames
    int saveGameSlot; //Where player choose to save/load to/from
    texture tex[5];
    GLuint dl;

    bool themeChanged; //If the theme has changed change the banner.
    vector<themeInfo> tI; //Vector of theme info

public:
    char highScores[20][255];
    bool joystickAttached; // Is joystick attached?

    menuClass() {
        refreshHighScoreList(); //load the highscore file (if exists)
        TextureManager texMgr;

        dl = glGenLists(4); //Generate displaylists (+0 = background, +1,2 = but

        texMgr.load(themeManager.getThemeFilePath("gfx/menu/menu0.png", setting.gfxTheme), tex[0]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/but0.png", setting.gfxTheme), tex[1]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/but1.png", setting.gfxTheme), tex[2]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/but2.png", setting.gfxTheme), tex[3]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/highscorebg.png", setting.gfxTheme), tex[4]);

        tI = themeManager.listThemes(); //Read themes and put them in the vector tI
        saveManager.listSaveGames(saveGameName);

        // Menu-Hintergrund
        glNewList(dl, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, tex[0].prop.texture);
        glColor4f(GL_WHITE);
        glBegin(GL_QUADS);

        // top positive
        // top left
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-0.8f, 0.8f, 0.0f);
        // top right
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(0.8f, 0.8f, 0.0f);
        // bottom right
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.8f, -0.8f, 0.0f);
        // bottom left
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-0.8f, -0.8f, 0.0f);
        glEnd();
        glEndList();

        // bläulich
        float half_width = 0.425f;
        glNewList(dl + 1, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, tex[1].prop.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.132f, 0.3f);
        glVertex3f(-half_width, 0.07, 0.0);
        glTexCoord2f(0.87f, 0.3f);
        glVertex3f(half_width, 0.07, 0.0);
        glTexCoord2f(0.87f, 0.7f);
        glVertex3f(half_width, -0.07, 0.0);
        glTexCoord2f(0.132f, 0.7f);
        glVertex3f(-half_width, -0.07, 0.0);
        glEnd();
        glEndList();

        // rötlich
        glNewList(dl + 2, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, tex[2].prop.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.132f, 0.3f);
        glVertex3f(-half_width, 0.07, 0.0);
        glTexCoord2f(0.87f, 0.3f);
        glVertex3f(half_width, 0.07, 0.0);
        glTexCoord2f(0.87f, 0.7f);
        glVertex3f(half_width, -0.07, 0.0);
        glTexCoord2f(0.132f, 0.7f);
        glVertex3f(-half_width, -0.07, 0.0);
        glEnd();
        glEndList();

        // grünlich
        glNewList(dl + 3, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, tex[3].prop.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.132f, 0.3f);
        glVertex3f(-half_width, 0.07, 0.0);
        glTexCoord2f(0.87f, 0.3f);
        glVertex3f(half_width, 0.07, 0.0);
        glTexCoord2f(0.87f, 0.7f);
        glVertex3f(half_width, -0.07, 0.0);
        glTexCoord2f(0.132f, 0.7f);
        glVertex3f(-half_width, -0.07, 0.0);
        glEnd();
        glEndList();
    }

    void refreshHighScoreList() {
        int lines;

        score *final = sortScores(&lines);

        //Fill out remaining highscore slots (if any)
        for (int t = lines; t < 20; t++) {
            switch (t) {
                case 0:
                    sprintf(highScores[t], "SDL-BALL v " VERSION);
                    break;
                case 1:
                    sprintf(highScores[t], "-----------------------");
                    break;
                case 2:
                    sprintf(highScores[t], "Copyleft GPLv3 2008-2014 Jimmy Christensen");
                    break;
                case 3:
                    sprintf(highScores[t], "Based on Dx-Ball by Michael P. Welch");
                    break;
                case 4:
                    sprintf(highScores[t], "Based on Megaball by Ed and Al Mackey");
                    break;
                case 5:
                    sprintf(highScores[t], "-----------------------");
                    break;
                case 6:
                    sprintf(highScores[t], "Thanks to players, packagers and patchers!");
                    break;
                case 7:
                    sprintf(highScores[t], "Greetings to everyone!");
                    break;
                default:
                    sprintf(highScores[t], "SDL-Ball - %i", (t * -1) + 20);
                    break;
            }
        }

        //Highscores list
        for (int t = 0; t < lines; t++) {
            sprintf(highScores[t], "%s%i - %s", final[t].level.data(), final[t].points, final[t].name.data());
        }

        if (lines > 0)
            delete[] final;
    }

    void enterSaveGameName(SDL_Event e) {
        if (e.key.keysym.sym != SDLK_RETURN && e.key.keysym.sym != SDLK_ESCAPE) {
            if (e.key.keysym.sym == SDLK_BACKSPACE) {
                if (!saveGameName[saveGameSlot].empty())
                    saveGameName[saveGameSlot].erase(saveGameName[saveGameSlot].length() - 1);
            } else {
                saveGameName[saveGameSlot] += e.key.keysym.sym;
            }
        } else {
            if (e.key.keysym.sym == SDLK_RETURN) {
                //player saved
                saveManager.saveGame(saveGameSlot, saveGameName[saveGameSlot], SOLPlayer);
            }
            var.enterSaveGameName = false;
        }
    }

    void doMenu() {

        glLoadIdentity();

        // Menu-background
        glCallList(dl);

        if (var.menu == 1) {

            // Quit SDL-Ball
            glPushMatrix();
            glTranslatef(0.0, -0.65f, 0.0f);
            glColor4f(GL_WHITE);
            if (var.menuItem == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_BLACK);
            glText->write("Quit SDL-Ball", FONT_MENU, true, 1.0, 0.0, -0.65f);

            constexpr float offset = 0.18f;

            // Settings
            glColor4f(GL_WHITE);
            glPushMatrix();
            glTranslatef(0.0, -0.65f+offset, 0.0f);
            if (var.menuItem == 2)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_BLACK);
            glText->write("Settings", FONT_MENU, true, 1.0, 0.0, -0.65f+offset);

            // Highscores
            glColor4f(GL_WHITE);
            glPushMatrix();
            glTranslatef(0.0, -0.65f+offset*2, 0.0f);
            if (var.menuItem == 3)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_BLACK);
            glText->write("Highscores", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*2);

            // Save Game
            glColor4f(GL_WHITE);
            glPushMatrix();
            glTranslatef(0.0, -0.65f+offset*3, 0.0f);
            if (player.level > 0) // && !var.startedPlaying)
            {
                if (var.menuItem == 4)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);


                glColor4f(GL_BLACK);
                glText->write("Save Game", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*3);
                glColor4f(GL_WHITE);
            } else {
                if (var.menuItem == 4) {
                    glCallList(dl + 2);
                    glColor4f(0.5, 0.5, 0.5, 1);
                    glText->write("Not in Level 1", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*3);
                } else {
                    glCallList(dl + 1);
                    glColor4f(0.5, 0.5, 0.5, 1);
                    glText->write("Save Game", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*3);
                }
                glPopMatrix();
            }

            // Load
            glColor4f(GL_WHITE);
            glPushMatrix();
            glTranslatef(0.0, -0.65f+offset*4, 0.0f);
            if (var.menuItem == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_BLACK);
            glText->write("Load Game", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*4);

            // Continue
            glColor4f(GL_WHITE);
            glPushMatrix();
            glTranslatef(0.0, -0.65f+offset*5, 0.0f);
            if (var.menuItem == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_BLACK);
            glText->write("Continue", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*5);

            // New game
            glColor4f(GL_WHITE);
            glPushMatrix();
            glTranslatef(0.0, -0.65f+offset*6, 0.0f);
            if (var.menuItem == 7)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);
            glPopMatrix();

            glColor4f(GL_BLACK);
            glText->write("New Game", FONT_MENU, true, 1.0, 0.0, -0.65f+offset*6);
            glColor4f(GL_WHITE);


            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 1: //quit
                        var.menu = 6;
                        break;
                    case 2: //settings
                        var.menu = 2;
                        break;
                    case 3: //highscores
                        var.menu = 7;
                        break;
                    case 4: //Save Game
                        if (player.level > 0) // && !var.startedPlaying)
                            var.menu = 9;
                        break;
                    case 5: //Load game
                        var.menu = 8;
                        break;
                    case 6: //resume
                        resumeGame();
                        break;
                    case 7: //new game
                        var.menu = 5;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 7;
        } else if (var.menu == 2) {
            //Settings

            // Back
            glTranslatef(0.0, 0.22, 0.0f);
            if (var.menuItem == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Settings
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);

            glCallList(dl + 3);

            glColor4f(GL_WHITE);
            glText->write("Settings", FONT_MENU, 1, 1.0, 0.0, -0.005);


            // Video
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("Video", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);


            // Sound
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("Sound", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Calibrate
            glTranslatef(0.0, -0.22, 0.0f);
            if (joystickAttached) {
                if (var.menuItem == 4)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                glText->write("Calibrate Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 2)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("Themes", FONT_MENU, true, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 1:
                        var.menu = 1;
                        break;
                    case 6:
                        var.menu = 3;
                        break;
                    case 5:
                        var.menu = 4;
                        break;
                    case 4:
                        if (joystickAttached) {
                            var.menuJoyCalStage = 0;
                            var.menu = 10;
                        }
                        break;
                    case 2:
                        var.menu = 12;
                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 6;
        } else if (var.menu == 3) {

            // Video options
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);

            glColor4f(GL_WHITE);
            glText->write("Video Options", FONT_MENU, 1, 1.0, 0.0, -0.005);

            // Toggle full
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.fullscreen)
                glText->write("Fullscreen:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            else
                glText->write("Fullscreen:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Eyecandy
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.eyeCandy)
                glText->write("Eyecandy:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            else
                glText->write("Eyecandy:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // 1600
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 4)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("1600x1200", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // 1024
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 3)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("1024x768", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // background
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 2)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.showBg)
                glText->write("Background:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            else
                glText->write("Background:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Back
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 1:
                        var.menu = 2;
                        break;
                    case 2:
                        if (!setting.showBg)
                            setting.showBg = true;
                        else
                            setting.showBg = false;
                        settingsManager.settingsChanged();
                        break;
                    case 3:
                        setting.res_x = 1024;
                        setting.res_y = 768;
                        if (display.updateForMenu())
                            settingsManager.settingsChanged();
                        break;
                    case 4:
                        setting.res_x = 1600;
                        setting.res_y = 1200;
                        if (display.updateForMenu())
                            settingsManager.settingsChanged();
                        break;
                    case 5:
                        if (setting.eyeCandy)
                            setting.eyeCandy = false;
                        else
                            setting.eyeCandy = true;
                        settingsManager.settingsChanged();
                        break;
                    case 6:
                        if (setting.fullscreen)
                            setting.fullscreen = false;
                        else
                            setting.fullscreen = true;
                        if (display.updateForMenu())
                            settingsManager.settingsChanged();
                        break;
                    default: ;
                }
                var.menuPressed = false;
            }
            var.menuNumItems = 6;
        } else if (var.menu == 4) {
            //Audio
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);
            glColor4f(GL_WHITE);
            glText->write("Audio", FONT_MENU, true, 1.0, 0.0, -0.005);

            // Sound on/off
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.sound)
                glText->write("Sound:On", FONT_MENU, true, 1.0, 0.0, -0.005);
            else
                glText->write("Sound:Off", FONT_MENU, true, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Stereo
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 5) {
                glCallList(dl + 2);
            } else {
                glCallList(dl + 1);
                glColor4f(GL_BLACK);
            }
            if (setting.stereo) {
                glText->write("Stereo:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else {
                glText->write("Stereo:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            }
            glColor4f(GL_WHITE);

            // Back
            glTranslatef(0.0, -0.88, 0.0f);
            if (var.menuItem == 1) {
                glCallList(dl + 2);
            } else {
                glCallList(dl + 1);
            }
            glColor4f(GL_BLACK);
            glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 1:
                        var.menu = 2;
                        break;
                    case 5:
                        if (setting.stereo)
                            setting.stereo = false;
                        else
                            setting.stereo = true;
                        settingsManager.settingsChanged();
                        break;
                    case 6:
                        if (setting.sound) {
                            setting.sound = false;
                        } else {
                            setting.sound = true;
                            soundMan.loadSounds();
                        }
                        settingsManager.settingsChanged();

                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }

            var.menuNumItems = 7;
        } else if (var.menu == 5) {
            // New game?
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);

            glColor4f(GL_BLACK);
            glText->write("New Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
            //       glBindTexture(GL_TEXTURE_2D, tex[28].prop.texture);
            //       glCallList(dl+3);

            // Easy
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_GREEN);
            glText->write("Easy", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
            //       glBindTexture(GL_TEXTURE_2D, tex[29].prop.texture);
            //       glCallList(dl+3);

            // Normal
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_BLUE);
            glText->write("Normal", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            //       glBindTexture(GL_TEXTURE_2D, tex[30].prop.texture);
            //       glCallList(dl+3);

            // Hard
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 4)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_RED);
            glText->write("Hard", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
            //       glBindTexture(GL_TEXTURE_2D, tex[31].prop.texture);
            //       glCallList(dl+3);

            // Back
            glTranslatef(0.0, -0.66, 0.0f);
            if (var.menuItem == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            //       glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
            //       glCallList(dl+3);

            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 1:
                        var.menu = 1;
                        break;
                    case 4:
                        player.difficulty = HARD;
                        settingsManager.settingsChanged();
                        resumeGame();
                        initNewGame();
                        break;
                    case 5:
                        player.difficulty = NORMAL;
                        settingsManager.settingsChanged();
                        resumeGame();
                        initNewGame();
                        break;
                    case 6:
                        player.difficulty = EASY;
                        settingsManager.settingsChanged();
                        resumeGame();
                        initNewGame();
                        break;
                    default: ;
                }
                var.menuPressed = false;
            }

            var.menuNumItems = 7;
        } else if (var.menu == 6) {

            // Exit
            glLoadIdentity();
            glPushMatrix();
            glTranslatef(0.0f, 0.4f, 0.0f);
            glColor4f(GL_WHITE);
            glCallList(dl + 2);
            glPopMatrix();
            glColor4f(GL_BLACK);
            glText->write("Exit Game?", FONT_MENU, true, 1.0, 0.0, 0.4f);

            // Yes
            glPushMatrix();
            glTranslatef(0.0, 0.02f, 0.0f);
            glColor4f(GL_WHITE);
            if (var.menuItem == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();
            glColor4f(GL_FULL_RED);
            glText->write("Yes.", FONT_MENU, true, 1.0, 0.0, 0.02f);
            glColor4f(GL_WHITE);

            // Noes!
            glPushMatrix();
            glTranslatef(0.0, -0.15, 0.0f);
            glColor4f(GL_WHITE);
            if (var.menuItem == 4)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_FULL_GREEN);
            glText->write("No way!", FONT_MENU, true, 1.0, 0.0, -0.15);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 4: //no
                        var.menu = 1;
                        break;
                    case 5: //Yes
                        var.quit = 1;
                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 5;
        } else if (var.menu == 7) // highscores
        {
            // Highscores
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            if (var.menuItem == 7)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);

            glColor4f(GL_WHITE);
            glText->write("Highscores", FONT_MENU, 1, 1.0, 0.0, -0.005);

            glTranslatef(0.0, -0.75, 0.0f);
            glBindTexture(GL_TEXTURE_2D, tex[4].prop.texture);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1.0, 1.0, 1.0, 1.0);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-0.8, 0.63, 0.0);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(0.8, 0.63, 0.0);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(0.8, -0.63, 0.0);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(-0.8, -0.63, 0.0);
            glEnd();

            glLoadIdentity();
            glTranslatef(0.0, 0.435, 0.0f);

            //Find out how many lines we have room for in the box.
            int nl = (1.26 / glText->getHeight(FONT_MENUHIGHSCORE));
            //If theres room for more than 20, only show 20..
            if (nl > 20) {
                nl = 20;
            }

            for (int i = 0; i < nl; i++) {
                glTranslatef(0.0, -glText->getHeight(FONT_MENUHIGHSCORE), 0.0f);
                glText->write(highScores[i], FONT_MENUHIGHSCORE, 0, 1.0, -0.75, 0.0);
            }


            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 7:
                        var.menu = 1;
                        break;
                    default: ;
                }
                var.menuPressed = false;
            }
        } else if (var.menu == 8) {
            // Load game
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            if (var.menuItem == 8)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);
            glColor4f(GL_WHITE);
            glText->write("Load Game", FONT_MENU, true, 1.0, 0.0, -0.005);

            for (int i = 0; i < 6; i++) {
                glTranslatef(0.0, -0.22, 0.0f);
                if (var.menuItem == 6 - i)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                glText->write(saveGameName[i], FONT_MENU, true, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            if (var.menuPressed) {
                if (var.menuItem == 7) {
                    var.menu = 1;
                } else if (var.menuItem != 0) {
                    saveManager.loadGame(var.menuItem * -1 + 6, SOLPlayer);
                    var.menu = 0;
                    resumeGame();
                }
                var.menuPressed = 0;
            }
        } else if (var.menu == 9) {
            // Save game
            // Back

            if (var.enterSaveGameName) {
                glLoadIdentity();
                glTranslatef(0.0, 0.54, 0.0f);
                glTranslatef(0.0, 0.12, 0.0f);

                glColor4f(GL_BLACK);
                glText->write("Enter name and press Enter to save. ESC to cancel", FONT_MENU, true, 1.0, 0.0, -0.005);
                glColor4f(1, 0, 0, 1);
            }

            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);

            if (var.menuItem == 8)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);
            glColor4f(GL_WHITE);
            glText->write("Save Game", FONT_MENU, true, 1.0, 0.0, -0.005);

            for (int i = 0; i < 6; i++) {
                glTranslatef(0.0, -0.22, 0.0f);
                if (var.menuItem == 6 - i)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                glText->write(saveGameName[i], FONT_MENU, true, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            if (var.menuPressed) {
                if (var.menuItem == 7) {
                    var.menu = 1;
                } else if (var.menuItem != 0) {
                    var.enterSaveGameName = true;
                    saveGameSlot = (var.menuItem * -1 + 6);
                    saveGameName[saveGameSlot] = "";
                }
                var.menuPressed = false;
            }
        } else if (var.menu == 10) {
            // Joystick options
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);
            glColor4f(GL_WHITE);
            glText->write("Calibrate Joystick", FONT_MENU, true, 1.0, 0.0, -0.005);


            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItem == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.joyIsDigital) {
                glText->write("Digital Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else if (setting.joyIsPaddle) {
                glText->write("Paddle", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else {
                glText->write("Analog Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
            }
            glColor4f(GL_WHITE);


            glTranslatef(0.0, -0.22, 0.0f);
            if (!setting.joyIsDigital) {
                if (var.menuItem == 5)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);

                glColor4f(GL_BLACK);
                if (var.menuJoyCalStage == 0)
                    glText->write("Start Calibration", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 1)
                    glText->write("Center, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 2)
                    glText->write("Move Left, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 3)
                    glText->write("Move Right, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 4)
                    glText->write("Saving...", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 5)
                    glText->write("Finished.", FONT_MENU, 1, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);

            if (var.menuItem == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItem) {
                    case 1:
                        var.menu = 2;
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    case 5:
                        if (!var.menuJoyCalStage)
                            var.menuJoyCalStage = 1;
                        break;
                    case 6:
                        if (setting.joyIsDigital) {
                            setting.joyIsPaddle = true;
                            setting.joyIsDigital = false;
                        } else if (setting.joyIsPaddle) {
                            setting.joyIsDigital = false;
                            setting.joyIsPaddle = false;
                        } else {
                            setting.joyIsDigital = true;
                        }
                        settingsManager.settingsChanged();
                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 6;
        }
        else if (var.menu == 12) //Theme selector (Main screen)
        {
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);

            if (!themeChanged) {
                glColor4f(GL_BLACK);
                glText->write("Themes", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else {
                glColor4f(1, 0, 0, 1);
                glText->write("Restart to apply", FONT_MENU, 1, 1.0, 0.0, -0.005);
            }
            glColor4f(GL_WHITE);

            int i = 0;
            for (vector<themeInfo>::iterator it = tI.begin(); it < tI.end(); ++it) {
                if (i == 5)
                    break;
                glTranslatef(0.0, -0.22, 0.0f);
                if (var.menuItem == 6 - i)
                    glCallList(dl + 2);
                else if (it->name.compare(setting.gfxTheme) == 0)
                    glCallList(dl + 3);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                glText->write(it->name, FONT_MENU, 1, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
                i++;
            }

            for (int a = i; a < 6; a++) {
                glTranslatef(0.0, -0.22, 0.0f);
            }

            if (var.menuItem == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
        }

        if (var.menuPressed) {
            int i = 0;
            switch (var.menuItem) {
                case 1:
                    var.menu = 2;
                    break;
                default: ;
            }

            for (i = 0; i < static_cast<int>(tI.size()) && i < 5; i++) {
                if (6 - i == var.menuItem) {
                    setting.gfxTheme = tI.at(i).name;
                    setting.sndTheme = tI.at(i).name;
                    setting.lvlTheme = tI.at(i).name;
                    themeChanged = true;
                    settingsManager.settingsChanged();
                }
            }
            var.menuPressed = false;
        }
    }
};
