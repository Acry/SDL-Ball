#include "SpriteSheetAnimation.h"
#include "TextManager.h"
#include "game_state.h"
// #include "Menu.h" todo

extern SettingsManager settingsManager;
extern SaveGameManager saveGameManager;
extern player_struct SOLPlayer;

// TODO: HighscoreManager
vector<score> sortScores() {
    vector<score> scores;
    ifstream hsList(configFileManager.getHighScoreFile());

    if (hsList.is_open()) {
        string line;
        while (getline(hsList, line)) {
            if (line.empty()) continue;

            score entry;
            size_t delim[2];

            if (line[0] == '[') {
                delim[0] = line.find(']');
                delim[1] = line.find('|', delim[0] + 1);

                entry.level = line.substr(0, delim[0] + 1) + "  ";
                entry.points = stoi(line.substr(delim[0] + 1, delim[1] - delim[0] - 1));
                entry.name = line.substr(delim[1] + 1);
            } else {
                delim[0] = line.find('|');
                entry.points = stoi(line.substr(0, delim[0]));
                entry.name = line.substr(delim[0] + 1);
            }
            scores.push_back(entry);
        }
    }

    sort(scores.begin(), scores.end(),
         [](const score& a, const score& b) { return a.points > b.points; });

    if (scores.size() > 20) {
        scores.resize(20);
    }

    return scores;
}

class Menu {
    TextManager& text;  // Referenz auf das Singleton
    string saveGameName[6]; //The name of saveGames
    int saveGameSlot; //Where player choose to save/load to/from
    SpriteSheetAnimation tex[5];
    GLuint dl;

    bool themeChanged; //If the theme has changed change the banner.
    vector<themeInfo> themeInf; //Vector of theme info

public:
    char highScores[20][255];
    bool joystickAttached; // Is joystick attached?

    Menu() : text(TextManager::getInstance()) {
        refreshHighScoreList(); //load the highscore file (if exists)
        const TextureManager texMgr;

        dl = glGenLists(4); //Generate displaylists (+0 = background, +1,2 = but

        texMgr.load(themeManager.getThemeFilePath("gfx/menu/menu0.png", setting.gfxTheme), tex[0]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/but0.png", setting.gfxTheme), tex[1]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/but1.png", setting.gfxTheme), tex[2]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/but2.png", setting.gfxTheme), tex[3]);
        texMgr.load(themeManager.getThemeFilePath("gfx/menu/highscorebg.png", setting.gfxTheme), tex[4]);

        themeInf = themeManager.listThemes(); //Read themes and put them in the vector tI
        saveGameManager.listSaveGames(saveGameName);

        // Hauptmenu-Rahmen
        glNewList(dl, GL_COMPILE);
        glEnable( GL_TEXTURE_2D );
        glBindTexture(GL_TEXTURE_2D, tex[0].textureProperties.texture);
        glColor4f(GL_WHITE);
        glBegin(GL_QUADS);
        constexpr float half_width = 0.5f;
        constexpr float half_height = 0.775f;
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-half_width, half_height, 0.0f);
        // top right
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(half_width, half_height, 0.0f);
        // bottom right
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(half_width, -half_height, 0.0f);
        // bottom left
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-half_width, -half_height, 0.0f);
        glEnd();
        glDisable( GL_TEXTURE_2D );
        glEndList();

        // Menupunkt - bläulich
        glNewList(dl + 1, GL_COMPILE);
        glEnable( GL_TEXTURE_2D );
        glBindTexture(GL_TEXTURE_2D, tex[1].textureProperties.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.132f, 0.3f);
        glVertex3f(-MENU_HALF_WIDTH, MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.87f, 0.3f);
        glVertex3f(MENU_HALF_WIDTH, MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.87f, 0.7f);
        glVertex3f(MENU_HALF_WIDTH, -MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.132f, 0.7f);
        glVertex3f(-MENU_HALF_WIDTH, -MENU_HALF_HEIGHT, 0.0);
        glEnd();
        glDisable( GL_TEXTURE_2D );
        glEndList();

        // Menupunkt - rötlich
        glNewList(dl + 2, GL_COMPILE);
        glEnable( GL_TEXTURE_2D );
        glBindTexture(GL_TEXTURE_2D, tex[2].textureProperties.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.132f, 0.3f);
        glVertex3f(-MENU_HALF_WIDTH, MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.87f, 0.3f);
        glVertex3f(MENU_HALF_WIDTH, MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.87f, 0.7f);
        glVertex3f(MENU_HALF_WIDTH, -MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.132f, 0.7f);
        glVertex3f(-MENU_HALF_WIDTH, -MENU_HALF_HEIGHT, 0.0);
        glEnd();
        glDisable( GL_TEXTURE_2D );
        glEndList();

        // Menupunkt - grünlich
        glNewList(dl + 3, GL_COMPILE);
        glEnable( GL_TEXTURE_2D );
        glBindTexture(GL_TEXTURE_2D, tex[3].textureProperties.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.132f, 0.3f);
        glVertex3f(-MENU_HALF_WIDTH, MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.87f, 0.3f);
        glVertex3f(MENU_HALF_WIDTH, MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.87f, 0.7f);
        glVertex3f(MENU_HALF_WIDTH, -MENU_HALF_HEIGHT, 0.0);
        glTexCoord2f(0.132f, 0.7f);
        glVertex3f(-MENU_HALF_WIDTH, -MENU_HALF_HEIGHT, 0.0);
        glEnd();
        glDisable( GL_TEXTURE_2D );
        glEndList();
    }

    // TODO: HighscoreManager
    void refreshHighScoreList() {
        constexpr int lines = 0;
        const vector<score> scores = sortScores();
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
        int t = 0;
        for (const auto& s : scores) {
            snprintf(highScores[t], sizeof(highScores[t]), "%s%i - %s",
                     s.level.c_str(), s.points, s.name.c_str());
            t++;
        }
    }

    void enterSaveGameName(const SDL_Event &e) {
        if (e.key.keysym.sym != SDLK_RETURN && e.key.keysym.sym != SDLK_ESCAPE) {
            if (e.key.keysym.sym == SDLK_BACKSPACE) {
                if (!saveGameName[saveGameSlot].empty())
                    saveGameName[saveGameSlot].erase(saveGameName[saveGameSlot].length() - 1);
            } else {
                saveGameName[saveGameSlot] += e.key.keysym.sym;
            }
        } else {
            if (e.key.keysym.sym == SDLK_RETURN) {
                saveGameManager.saveGame(saveGameSlot, saveGameName[saveGameSlot], SOLPlayer);
            }
            var.enterSaveGameName = false;
        }
    }

    void doMenu() {

        glLoadIdentity();
        // Menu-background
        glCallList(dl);
        if (var.menuShown == 1) {

            // Quit SDL-Ball
            // ORDER: BOTTOM UP
            constexpr float start_bottom = -0.62f;
            glTranslatef(0.0, start_bottom, 0.0f);
            glColor4f(GL_WHITE);
            if (var.menuItemHovered == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            text.write("Quit SDL-Ball", FONT_MENU, true, 1.0, 0.0, start_bottom);


            // Settings
            glColor4f(GL_WHITE);
            glTranslatef(0.0, +MENU_SPACING, 0.0f);
            if (var.menuItemHovered == 2)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            text.write("Settings", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING);

            // Highscores
            glColor4f(GL_WHITE);
            glTranslatef(0.0, +MENU_SPACING, 0.0f);
            if (var.menuItemHovered == 3)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            text.write("Highscores", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*2);

            // Save Game
            glColor4f(GL_WHITE);
            glTranslatef(0.0, +MENU_SPACING, 0.0f);
            if (player.level > 0) // && !var.startedPlaying)
            {
                if (var.menuItemHovered == 4)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);

                glColor4f(GL_BLACK);
                text.write("Save Game", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*3);
                glColor4f(GL_WHITE);
            } else {
                if (var.menuItemHovered == 4) {
                    glCallList(dl + 2);
                    glColor4f(0.5, 0.5, 0.5, 1);
                    text.write("Not in Level 1", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*3);
                } else {
                    glCallList(dl + 1);
                    glColor4f(0.5, 0.5, 0.5, 1);
                    text.write("Save Game", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*3);
                }

            }

            // Load
            glColor4f(GL_WHITE);
            glTranslatef(0.0, MENU_SPACING, 0.0f);
            if (var.menuItemHovered == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            text.write("Load Game", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*4);

            // Continue
            glColor4f(GL_WHITE);
            glTranslatef(0.0, MENU_SPACING, 0.0f);
            if (var.menuItemHovered == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glPopMatrix();

            glColor4f(GL_BLACK);
            text.write("Continue", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*5);

            // New game
            glColor4f(GL_WHITE);
            glTranslatef(0.0, MENU_SPACING, 0.0f);
            if (var.menuItemHovered == 7)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);

            glColor4f(GL_BLACK);
            text.write("New Game", FONT_MENU, true, 1.0, 0.0, start_bottom+MENU_SPACING*6);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 1: //quit
                        var.menuShown = 6;
                        break;
                    case 2: //settings
                        var.menuShown = 2;
                        break;
                    case 3: //highscores
                        var.menuShown = 7;
                        break;
                    case 4: //Save Game
                        if (player.level > 0) // && !var.startedPlaying)
                            var.menuShown = 9;
                        break;
                    case 5: //Load game
                        var.menuShown = 8;
                        break;
                    case 6: //resume
                        resumeGame();
                        break;
                    case 7: //new game
                        var.menuShown = 5;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 7;
        } else if (var.menuShown == 2) {
            //Settings

            // Back
            glTranslatef(0.0, 0.22, 0.0f);
            if (var.menuItemHovered == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            text.write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Settings
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);

            glCallList(dl + 3);

            glColor4f(GL_WHITE);
            text.write("Settings", FONT_MENU, 1, 1.0, 0.0, -0.005);


            // Video
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("Video", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);


            // Sound
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("Sound", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Calibrate
            glTranslatef(0.0, -0.22, 0.0f);
            if (joystickAttached) {
                if (var.menuItemHovered == 4)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                text.write("Calibrate Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 2)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("Themes", FONT_MENU, true, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 1:
                        var.menuShown = 1;
                        break;
                    case 6:
                        var.menuShown = 3;
                        break;
                    case 5:
                        var.menuShown = 4;
                        break;
                    case 4:
                        if (joystickAttached) {
                            var.menuJoyCalStage = 0;
                            var.menuShown = 10;
                        }
                        break;
                    case 2:
                        var.menuShown = 12;
                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 6;
        } else if (var.menuShown == 3) {

            // Video options
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);

            glColor4f(GL_WHITE);
            text.write("Video Options", FONT_MENU, 1, 1.0, 0.0, -0.005);

            // Toggle full
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.fullscreen)
                text.write("Fullscreen:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            else
                text.write("Fullscreen:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Eyecandy
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.eyeCandy)
                text.write("Eyecandy:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            else
                text.write("Eyecandy:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // 1600
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 4)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("1600x1200", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // 1024
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 3)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("1024x768", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // background
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 2)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.showBg)
                text.write("Background:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            else
                text.write("Background:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Back
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 1:
                        var.menuShown = 2;
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
        } else if (var.menuShown == 4) {
            //Audio
            glLoadIdentity();
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);
            glColor4f(GL_WHITE);
            text.write("Audio", FONT_MENU, true, 1.0, 0.0, -0.005);

            // Sound on/off
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.sound)
                text.write("Sound:On", FONT_MENU, true, 1.0, 0.0, -0.005);
            else
                text.write("Sound:Off", FONT_MENU, true, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            // Stereo
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 5) {
                glCallList(dl + 2);
            } else {
                glCallList(dl + 1);
                glColor4f(GL_BLACK);
            }
            if (setting.stereo) {
                text.write("Stereo:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else {
                text.write("Stereo:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
            }
            glColor4f(GL_WHITE);

            // Back
            glTranslatef(0.0, -0.88, 0.0f);
            if (var.menuItemHovered == 1) {
                glCallList(dl + 2);
            } else {
                glCallList(dl + 1);
            }
            glColor4f(GL_BLACK);
            text.write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 1:
                        var.menuShown = 2;
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
                            soundManager.loadSounds();
                        }
                        settingsManager.settingsChanged();
                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }

            var.menuNumItems = 7;
        } else if (var.menuShown == 5) {
            // New game?
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);

            glColor4f(GL_BLACK);
            text.write("New Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
            //       glBindTexture(GL_TEXTURE_2D, tex[28].prop.texture);
            //       glCallList(dl+3);

            // Easy
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_GREEN);
            text.write("Easy", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
            //       glBindTexture(GL_TEXTURE_2D, tex[29].prop.texture);
            //       glCallList(dl+3);

            // Normal
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_BLUE);
            text.write("Normal", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            //       glBindTexture(GL_TEXTURE_2D, tex[30].prop.texture);
            //       glCallList(dl+3);

            // Hard
            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 4)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_RED);
            text.write("Hard", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
            //       glBindTexture(GL_TEXTURE_2D, tex[31].prop.texture);
            //       glCallList(dl+3);

            // Back
            glTranslatef(0.0, -0.66, 0.0f);
            if (var.menuItemHovered == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_BLACK);
            text.write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            //       glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
            //       glCallList(dl+3);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 1:
                        var.menuShown = 1;
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
        } else if (var.menuShown == 6) {

            // Exit
            glColor4f(GL_WHITE);
            glCallList(dl + 2);
            glColor4f(GL_BLACK);
            text.write("Exit Game?", FONT_MENU, true, 1.0, 0.0, 0.4f);

            // Yes

            glTranslatef(0.0, 0.02f, 0.0f);
            glColor4f(GL_WHITE);
            if (var.menuItemHovered == 5)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_FULL_RED);
            text.write("Yes.", FONT_MENU, true, 1.0, 0.0, 0.02f);
            glColor4f(GL_WHITE);

            // Noes!

            glTranslatef(0.0, -0.15, 0.0f);
            glColor4f(GL_WHITE);
            if (var.menuItemHovered == 4)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);

            glColor4f(GL_FULL_GREEN);
            text.write("No way!", FONT_MENU, true, 1.0, 0.0, -0.15);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 4: //no
                        var.menuShown = 1;
                        break;
                    case 5: //Yes
                        var.quit = 1;
                        break;
                    default: ;
                }
                var.menuPressed = 0;
            }
            var.menuNumItems = 5;
        } else if (var.menuShown == 7) // highscores
        {
            // Highscores
            glTranslatef(0.0, 0.54, 0.0f);
            if (var.menuItemHovered == 7)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);

            glColor4f(GL_WHITE);
            text.write("Highscores", FONT_MENU, 1, 1.0, 0.0, -0.005);

            glTranslatef(0.0, -0.75, 0.0f);
            glEnable( GL_TEXTURE_2D );
            glBindTexture(GL_TEXTURE_2D, tex[4].textureProperties.texture);
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
            glDisable( GL_TEXTURE_2D );
            glTranslatef(0.0, 0.435, 0.0f);

            //Find out how many lines we have room for in the box.
            int nl = (1.26 / text.getHeight(FONT_MENUHIGHSCORE));
            //If theres room for more than 20, only show 20..
            if (nl > 20) {
                nl = 20;
            }

            for (int i = 0; i < nl; i++) {
                glTranslatef(0.0, -text.getHeight(FONT_MENUHIGHSCORE), 0.0f);
                text.write(highScores[i], FONT_MENUHIGHSCORE, 0, 1.0, -0.75, 0.0);
            }


            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 7:
                        var.menuShown = 1;
                        break;
                    default: ;
                }
                var.menuPressed = false;
            }
        } else if (var.menuShown == 8) {
            // Load game
            glTranslatef(0.0, 0.54, 0.0f);
            if (var.menuItemHovered == 8)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);
            glColor4f(GL_WHITE);
            text.write("Load Game", FONT_MENU, true, 1.0, 0.0, -0.005);

            for (int i = 0; i < 6; i++) {
                glTranslatef(0.0, -0.22, 0.0f);
                if (var.menuItemHovered == 6 - i)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                text.write(saveGameName[i], FONT_MENU, true, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            if (var.menuPressed) {
                if (var.menuItemHovered == 7) {
                    var.menuShown = 1;
                } else if (var.menuItemHovered != 0) {
                    saveGameManager.loadGame(var.menuItemHovered * -1 + 6, SOLPlayer);
                    var.menuShown = 0;
                    resumeGame();
                }
                var.menuPressed = false;
            }
        } else if (var.menuShown == 9) {
            if (var.enterSaveGameName) {
                glLoadIdentity();
                glTranslatef(0.0, 0.54, 0.0f);
                glTranslatef(0.0, 0.12, 0.0f);

                glColor4f(GL_BLACK);
                text.write("Enter name and press Enter to save. ESC to cancel", FONT_MENU, true, 1.0, 0.0, -0.005);
                glColor4f(1, 0, 0, 1);
            }

            glTranslatef(0.0, 0.54, 0.0f);

            if (var.menuItemHovered == 8)
                glCallList(dl + 2);
            else
                glCallList(dl + 3);
            glColor4f(GL_WHITE);
            text.write("Save Game", FONT_MENU, true, 1.0, 0.0, -0.005);

            for (int i = 0; i < 6; i++) {
                glTranslatef(0.0, -0.22, 0.0f);
                if (var.menuItemHovered == 6 - i)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                text.write(saveGameName[i], FONT_MENU, true, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }
            if (var.menuPressed) {
                if (var.menuItemHovered == 7) {
                    var.menuShown = 1;
                } else if (var.menuItemHovered != 0) {
                    var.enterSaveGameName = true;
                    saveGameSlot = (var.menuItemHovered * -1 + 6);
                    saveGameName[saveGameSlot] = "";
                }
                var.menuPressed = false;
            }
        } else if (var.menuShown == 10) {
            // Joystick options
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);
            glColor4f(GL_WHITE);
            text.write("Calibrate Joystick", FONT_MENU, true, 1.0, 0.0, -0.005);

            glTranslatef(0.0, -0.22, 0.0f);
            if (var.menuItemHovered == 6)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            if (setting.joyIsDigital) {
                text.write("Digital Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else if (setting.joyIsPaddle) {
                text.write("Paddle", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else {
                text.write("Analog Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
            }
            glColor4f(GL_WHITE);
            glTranslatef(0.0, -0.22, 0.0f);
            if (!setting.joyIsDigital) {
                if (var.menuItemHovered == 5)
                    glCallList(dl + 2);
                else
                    glCallList(dl + 1);

                glColor4f(GL_BLACK);
                if (var.menuJoyCalStage == 0)
                    text.write("Start Calibration", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 1)
                    text.write("Center, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 2)
                    text.write("Move Left, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 3)
                    text.write("Move Right, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 4)
                    text.write("Saving...", FONT_MENU, 1, 1.0, 0.0, -0.005);
                else if (var.menuJoyCalStage == 5)
                    text.write("Finished.", FONT_MENU, 1, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
            }

            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);
            glTranslatef(0.0, -0.22, 0.0f);

            if (var.menuItemHovered == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);

            if (var.menuPressed) {
                switch (var.menuItemHovered) {
                    case 1:
                        var.menuShown = 2;
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
        else if (var.menuShown == 12) //Theme selector (Main screen)
        {
            glTranslatef(0.0, 0.54, 0.0f);
            glCallList(dl + 3);

            if (!themeChanged) {
                glColor4f(GL_BLACK);
                text.write("Themes", FONT_MENU, 1, 1.0, 0.0, -0.005);
            } else {
                glColor4f(1, 0, 0, 1);
                text.write("Restart to apply", FONT_MENU, 1, 1.0, 0.0, -0.005);
            }
            glColor4f(GL_WHITE);

            int i = 0;
            for (vector<themeInfo>::iterator it = themeInf.begin(); it < themeInf.end(); ++it) {
                if (i == 5)
                    break;
                glTranslatef(0.0, -0.22, 0.0f);
                if (var.menuItemHovered == 6 - i)
                    glCallList(dl + 2);
                else if (it->name.compare(setting.gfxTheme) == 0)
                    glCallList(dl + 3);
                else
                    glCallList(dl + 1);
                glColor4f(GL_BLACK);
                text.write(it->name, FONT_MENU, 1, 1.0, 0.0, -0.005);
                glColor4f(GL_WHITE);
                i++;
            }

            for (int a = i; a < 6; a++) {
                glTranslatef(0.0, -0.22, 0.0f);
            }

            if (var.menuItemHovered == 1)
                glCallList(dl + 2);
            else
                glCallList(dl + 1);
            glColor4f(GL_BLACK);
            text.write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
            glColor4f(GL_WHITE);
        }

        if (var.menuPressed) {
            int i = 0;
            switch (var.menuItemHovered) {
                case 1:
                    var.menuShown = 2;
                    break;
                default: ;
            }

            for (i = 0; i < static_cast<int>(themeInf.size()) && i < 5; i++) {
                if (6 - i == var.menuItemHovered) {
                    setting.gfxTheme = themeInf.at(i).name;
                    setting.sndTheme = themeInf.at(i).name;
                    setting.lvlTheme = themeInf.at(i).name;
                    themeChanged = true;
                    settingsManager.settingsChanged();
                }
            }
            var.menuPressed = false;
        }
    }
};
