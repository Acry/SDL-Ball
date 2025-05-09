#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <memory>
#include <random>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "SaveFileManager.h"
#include "colors.h"
#include "config.h"
#include "config_file.h"
#include "display.hpp"
#include "settings_manager.h"
#include "sound.h"
#include "text.h"
#include "ThemeManager.h"

using namespace std;

settings setting;
ConfigFile configFile;
SettingsManager settingsManager(configFile);
SaveFileManager saveManager(configFile);
ThemeManager themeManager(configFile);

class effectManager;
class ball;
class paddle_class;

gameVars gVar;
// current player
player_struct player;
// start of round player - for save game
player_struct SOLPlayer;
vars var;
displayClass display;

soundClass soundMan; //Public object so all objects can use it
//#define debugBall 1
//     #define DEBUG_DRAW_BALL_QUAD
//     #define DEBUG_NO_RELATIVE_MOUSE
//     #define DEBUG_SHOW_MOUSE_COORDINATES

int globalTicks;
float globalMilliTicks;
Uint32 nonpausingGlobalTicks;
float nonpausingGlobalMilliTicks;
int globalTicksSinceLastDraw;
float globalMilliTicksSinceLastDraw;

struct pos {
    GLfloat x;
    GLfloat y;
};

struct difficultyStruct {
    GLfloat ballspeed[3];
    GLfloat maxballspeed[3];
    GLfloat hitbrickinc[3];
    GLfloat hitpaddleinc[3];
    GLfloat slowdown[3];
    GLfloat speedup[3];
};

difficultyStruct static_difficulty, difficulty;

struct texProp {
    GLuint texture; // Den GLtexture der er loaded
    GLfloat xoffset; // Hvor stort er springet mellem hver subframe
    GLfloat yoffset; //
    int cols, rows; // hvor mange rækker og kolonner er der i denne textur
    int ticks;
    uint frames; // This many frames in each se
    bool bidir; // Går Looper den fra 0 -> X - 0 eller fra 0 -> X -> 0
    bool playing;

    bool padding;
    // Bit of a nasty hack, but if a texture is padded with 1 pixel around each frame, this have to be set to 1
    float pxw, pxh; // pixel width, and height

    GLfloat glTexColorInfo[4];
    GLfloat glParColorInfo[3]; // This and above replaced object::color and particle colors

    string fileName; // Quite the fugly.. This will be set by readTexProps();
};

void initNewGame();
void pauseGame();
void resumeGame();
float random_float(float total, float negative);
float bounceOffAngle(GLfloat width, GLfloat posx, GLfloat hitx);

typedef GLfloat texPos[8];
#ifndef uint // WIN32
typedef unsigned int uint;
#endif

glTextClass *glText;
// Pointer to the object, since we can't init (load fonts) because the settings have not been read yet.

class textureClass {
private:
    float age; //Hvor gammel er den frame vi er ved?
    bool dir; //hvis dette er en animation der går frem og tilbage hvilken retning

    uint lastframe; //check om det er den samme frame som sidst, så vi kan vide om vis skal opdatere cords

public:
    uint frame; //hvilken frame er vi nået til i texturen (den er public så vi kan lave offset)
    bool playing; //spiller vi?
    bool firstFrame; //If this is the first frame
    texPos pos; //Kordinater for den frame på texturen der er nu
    texProp prop; //Properties for den textur som dette objekt har

    textureClass() {
        age = 10000;
        firstFrame = true;
        lastframe = 1000;
        frame = 1;
        dir = false;
    }

    void play() {
        if (prop.playing) {
            //Skal vi skifte frame?
            age += globalTicksSinceLastDraw;
            if (age >= prop.ticks) //Denne frame har været vist længe nok
            {
                age = 0.0;
                if (!dir) {
                    if (frame == prop.frames) {
                        if (prop.bidir) {
                            dir = true;
                        } else {
                            frame = 1;
                        }
                    } else {
                        frame++;
                    }
                }

                if (dir) {
                    if (frame == 1) {
                        dir = false;
                        frame = 2;
                    } else {
                        frame--;
                    }
                }
            }
        }

        if (frame != lastframe || firstFrame) {
            int row = 0;
            uint f = 0;
            int col = 0;
            lastframe = frame;
            firstFrame = false;

            //hvor mange kolonner er der på en række

            for (row = 0; row < prop.rows; row++) {
                for (col = 0; col < prop.cols; col++) {
                    f++;
                    if (f == frame) {
                        //Øverst Venstre
                        pos[0] = (prop.xoffset * static_cast<float>(col)); //0.0;
                        pos[1] = (prop.yoffset * static_cast<float>(row)); //0.0;

                        //Øverst højre
                        pos[2] = (prop.xoffset * static_cast<float>(col)) + prop.xoffset;
                        pos[3] = (prop.yoffset * static_cast<float>(row)); //0.0;

                        //Nederst højre
                        pos[4] = (prop.xoffset * static_cast<float>(col)) + prop.xoffset; // 1
                        pos[5] = (prop.yoffset * static_cast<float>(row)) + prop.yoffset; // 1

                        //Nederst venstre
                        pos[6] = (prop.xoffset * static_cast<float>(col)); //0.0;
                        pos[7] = (prop.yoffset * static_cast<float>(row)) + prop.yoffset; //1

                        if (prop.padding) {
                            pos[0] += 1.0 / prop.pxw;
                            pos[1] += 1.0 / prop.pxh;

                            pos[2] -= 1.0 / prop.pxw;
                            pos[3] += 1.0 / prop.pxh;

                            pos[4] -= 1.0 / prop.pxw;
                            pos[5] -= 1.0 / prop.pxh;

                            pos[6] += 1.0 / prop.pxw;
                            pos[7] -= 1.0 / prop.pxh;
                        }
                    }
                }
            }
        }
    }
};

/* This function reads textureProperties from fileName
   and applies them to *tex */
class textureManager {
public:
    bool load(const string &file, textureClass &tex) {
        SDL_Surface *temp = nullptr;
        GLint maxTexSize;
        GLuint glFormat = GL_RGBA;

        if (file.substr(file.length() - 3, 3).compare("jpg") == 0) {
            glFormat = GL_RGB;
        }

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

        temp = IMG_Load(file.data());

        if (temp == nullptr) {
            SDL_Log("Texture manager:%s :%s", file.c_str(), SDL_GetError());
            SDL_FreeSurface(temp);
            return false;
        }


        //Hvis større end tilladt:
        if (temp->w > maxTexSize) {
            SDL_Log("Texture manager: '%s' texturesize too large.", file.c_str());
            SDL_FreeSurface(temp);
            return false;
        }


        glGenTextures(1, &tex.prop.texture);
        glBindTexture(GL_TEXTURE_2D, tex.prop.texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, glFormat, temp->w, temp->h, 0, glFormat, GL_UNSIGNED_BYTE, temp->pixels);

        tex.prop.pxw = temp->w;
        tex.prop.pxh = temp->h;
        SDL_FreeSurface(temp);

        return true;
    }

    void readTexProps(string fileName, textureClass &tex) {
        char rgba[4][5];
        ifstream f;
        string set, val;
        f.open(fileName.data());

        if (f.is_open()) {
            string line;
            while (!f.eof()) {
                getline(f, line);
                if (line.find('=') != string::npos) {
                    set = line.substr(0, line.find('='));
                    val = line.substr(line.find('=') + 1);
                    if (set == "xoffset") {
                        tex.prop.xoffset = atof(val.data());
                    } else if (set == "yoffset") {
                        tex.prop.yoffset = atof(val.data());
                    } else if (set == "cols") {
                        tex.prop.cols = atoi(val.data());
                    } else if (set == "rows") {
                        tex.prop.rows = atoi(val.data());
                    } else if (set == "ticks") {
                        tex.prop.ticks = atoi(val.data());
                    } else if (set == "frames") {
                        tex.prop.frames = atoi(val.data());
                    } else if (set == "bidir") {
                        tex.prop.bidir = atoi(val.data());
                    } else if (set == "playing") {
                        tex.prop.playing = atoi(val.data());
                    } else if (set == "padding") {
                        tex.prop.padding = atoi(val.data());
                    } else if (set == "texcolor") {
                        //Color in hex RGBA
                        //Example:color=FFFFFFFF
                        sprintf(rgba[0], "0x%c%c", val[0], val[1]);
                        sprintf(rgba[1], "0x%c%c", val[2], val[3]);
                        sprintf(rgba[2], "0x%c%c", val[4], val[5]);
                        sprintf(rgba[3], "0x%c%c", val[6], val[7]);
                        tex.prop.glTexColorInfo[0] = 0.003921569 * strtol(rgba[0], nullptr, 16);
                        tex.prop.glTexColorInfo[1] = 0.003921569 * strtol(rgba[1], nullptr, 16);
                        tex.prop.glTexColorInfo[2] = 0.003921569 * strtol(rgba[2], nullptr, 16);
                        tex.prop.glTexColorInfo[3] = 0.003921569 * strtol(rgba[3], nullptr, 16);
                    } else if (set == "parcolor") {
                        //Color in hex RGBA
                        //Example:color=FFFFFFFF
                        sprintf(rgba[0], "0x%c%c", val[0], val[1]);
                        sprintf(rgba[1], "0x%c%c", val[2], val[3]);
                        sprintf(rgba[2], "0x%c%c", val[4], val[5]);
                        tex.prop.glParColorInfo[0] = 0.003921569 * strtol(rgba[0], nullptr, 16);
                        tex.prop.glParColorInfo[1] = 0.003921569 * strtol(rgba[1], nullptr, 16);
                        tex.prop.glParColorInfo[2] = 0.003921569 * strtol(rgba[2], nullptr, 16);
                    } else if (set == "file") {
                        tex.prop.fileName = val;
                    } else {
                        SDL_Log("Error: '%s' invalid setting '%s' with value '%s'", fileName.c_str(), set.c_str(),
                                val.c_str());
                    }
                }
            }

            //Load the texture if we have a filename.
            if (tex.prop.fileName.length() > 1) {
                string name = "gfx/" + tex.prop.fileName;
                load(themeManager.getThemeFilePath(name, setting.gfxTheme), tex);
            }
        } else {
            SDL_Log("readTexProps: Cannot open '%s'", fileName.c_str());
        }
    }
};

#include "menu.cpp"
#include "scoreboard.cpp"

class game_object {
public:
    //     GLfloat color[3];
    GLfloat opacity; // This is still used, because it can then be reset, and it's used for fading out bricks (i think)
    GLfloat posx, posy;
    GLfloat width, height;
    GLuint dl; // opengl display list
    bool active;
    bool collide;
    bool reflect; // NOTE: use this for bricks that are not going to reflect the ball? (trap brick? :D)

    textureClass tex;
};
class moving_object : public game_object {
public:
    GLfloat xvel, yvel, velocity;

    moving_object() {
        xvel = 0.0;
        yvel = 0.0;
    }
};

class paddle_class : public game_object {
    GLfloat growspeed;
    GLfloat destwidth;
    GLfloat aspect; // Verhältnis, um wie viel die Höhe zur Breite wächst.
    bool growing;

public:
    bool dead;
    textureClass *layerTex;

    paddle_class() {
        init();
        growing = false;
        growspeed = 0.05f;
        aspect = 0.2f;
    }

    void init() {
        posy = -0.9;
        width = 0.1;
        height = 0.02;
        dead = false;
    }

    void grow(const GLfloat width) {
        destwidth = width;
        growing = true;
    }

    void draw() {
        if (!dead) {
            if (player.powerup[PO_DIE]) {
                player.powerup[PO_DIE] = false;
                dead = true;
                width = 0.0;
                height = 0.0;
            }

            if (growing) {
                const GLfloat ix = growspeed * globalTicksSinceLastDraw;

                width += ix;

                if (width >= destwidth) {
                    width = destwidth;
                    height = aspect * destwidth;
                    growing = false;
                }
            }
            glLoadIdentity();
            //glPushMatrix();
            glTranslatef(posx, posy, 0.0);

            tex.play();
            glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
            glColor4f(tex.prop.glTexColorInfo[0], tex.prop.glTexColorInfo[1], tex.prop.glTexColorInfo[2],
                      tex.prop.glTexColorInfo[3]);
            glBegin(GL_QUADS);
            glTexCoord2f(tex.pos[0], tex.pos[1]);
            glVertex3f(-width, height, 0.0f);
            glTexCoord2f(tex.pos[2], tex.pos[3]);
            glVertex3f(width, height, 0.0f);
            glTexCoord2f(tex.pos[4], tex.pos[5]);
            glVertex3f(width, -height, 0.0f);
            glTexCoord2f(tex.pos[6], tex.pos[7]);
            glVertex3f(-width, -height, 0.0f);
            // glPopMatrix();

            // Hvis glue?
            if (player.powerup[PO_GLUE]) {
                glBindTexture(GL_TEXTURE_2D, layerTex[0].prop.texture);
                glColor4f(layerTex[0].prop.glTexColorInfo[0], layerTex[0].prop.glTexColorInfo[1],
                          layerTex[0].prop.glTexColorInfo[2], layerTex[0].prop.glTexColorInfo[3]);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-width, height, 0.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(width, height, 0.0f);
                glTexCoord2f(1.0f, 0.99f);
                glVertex3f(width, -height, 0.0f);
                glTexCoord2f(0.0f, 0.99f);
                glVertex3f(-width, -height, 0.0f);
                glEnd();
            }

            //Hvis gun
            if (player.powerup[PO_GUN]) {
                layerTex[1].play();
                glBindTexture(GL_TEXTURE_2D, layerTex[1].prop.texture);
                glColor4f(layerTex[1].prop.glTexColorInfo[0], layerTex[1].prop.glTexColorInfo[1],
                          layerTex[1].prop.glTexColorInfo[2], layerTex[1].prop.glTexColorInfo[3]);
                glBegin(GL_QUADS);
                glTexCoord2f(layerTex[1].pos[0], layerTex[1].pos[1]);
                glVertex3f(-width, height * 4, 0.0f);
                glTexCoord2f(layerTex[1].pos[2], layerTex[1].pos[3]);
                glVertex3f(width, height * 4, 0.0f);
                glTexCoord2f(layerTex[1].pos[4], layerTex[1].pos[5] - 0.01f);
                glVertex3f(width, height, 0.0f);
                glTexCoord2f(layerTex[1].pos[6], layerTex[1].pos[7] - 0.01f);
                glVertex3f(-width, height, 0.0f);
                glEnd();
            }
        }
    }
};

// nasty fix to a problem
int nbrick[23][26];
int updated_nbrick[23][26];
class brick;

void makeExplosive(brick &b);

textureClass *texExplosiveBrick; //NOTE:Ugly
class brick : public game_object {
public:
    int score; //Hvor meget gir den
    bool destroytowin; // Skal den smadres for at man kan vinde?
    char powerup;
    char type;
    GLfloat fade; //hvor meget brik
    GLfloat fadespeed;
    GLfloat zoomspeed;
    GLfloat zoom;
    bool isdyingnormally;
    bool isexploding; //springer den i luften
    int row; //what row is this brick in
    int bricknum; //brick in this row
    int hitsLeft; //Hvor mange gange skal denne brik rammes før den dør?
    bool justBecomeExplosive; //If this brick just become a explosive one.

    [[nodiscard]] bool n(const int dir) const {
        switch (dir) {
            case 0: //Er der en brik til venstre for dig?
                if (bricknum > 0) {
                    if (nbrick[row][bricknum - 1] != -1)
                        return true;
                }
                break;
            case 1: //Er der en brik til højre for dig?
                if (bricknum < 25) //26
                {
                    if (nbrick[row][bricknum + 1] != -1)
                        return true;
                }
                break;
            case 2: //Er der en brik Ovenpå dig
                if (row > 0) {
                    if (nbrick[row - 1][bricknum] != -1)
                        return true;
                }
                break;
            case 3: //Er der en brik nedenunder dig
                if (row < 22) //23
                {
                    if (nbrick[row + 1][bricknum] != -1)
                        return true;
                }
                break;
            default: ;
        }

        return false;
    }


    void hit(effectManager &fxMan, pos poSpawnPos, pos poSpawnVel, bool ballHitMe);

    void draw(brick bricks[], effectManager &fxMan) {
        if (isdyingnormally) {
            fade -= fadespeed * globalMilliTicksSinceLastDraw;
            opacity = fade;
            zoom -= zoomspeed * globalMilliTicksSinceLastDraw;

            if (fade < 0.0)
                active = false;
        }

        if (isexploding && !var.paused) {
            fade -= 7.0 * globalMilliTicksSinceLastDraw;
            opacity = fade;
            if (fade < 0.0) {
                active = false;

                pos spos, svel;
                spos.x = posx;
                spos.y = posy;


                if (bricknum > 0) {
                    if (nbrick[row][bricknum - 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row][bricknum - 1]].hit(fxMan, spos, svel, 0);
                    }
                }


                if (bricknum < 25) {
                    if (nbrick[row][bricknum + 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row][bricknum + 1]].hit(fxMan, spos, svel, 0);
                    }
                }

                if (row > 0) {
                    if (nbrick[row - 1][bricknum] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row - 1][bricknum]].hit(fxMan, spos, svel, 0);
                    }
                }

                if (row < 22) {
                    if (nbrick[row + 1][bricknum] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row + 1][bricknum]].hit(fxMan, spos, svel, 0);
                    }
                }

                if (row > 0 && bricknum > 0) {
                    if (nbrick[row - 1][bricknum - 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row - 1][bricknum - 1]].hit(fxMan, spos, svel, 0);
                    }
                }
                if (row > 0 && bricknum < 25) {
                    if (nbrick[row - 1][bricknum + 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row - 1][bricknum + 1]].hit(fxMan, spos, svel, 0);
                    }
                }

                if (row < 22 && bricknum > 0) {
                    if (nbrick[row + 1][bricknum - 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row + 1][bricknum - 1]].hit(fxMan, spos, svel, 0);
                    }
                }

                if (row < 22 && bricknum < 25) {
                    if (nbrick[row + 1][bricknum + 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row + 1][bricknum + 1]].hit(fxMan, spos, svel, 0);
                    }
                }
            }
        }

        tex.play();

        glColor4f(tex.prop.glTexColorInfo[0], tex.prop.glTexColorInfo[1], tex.prop.glTexColorInfo[2], opacity);

        glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(tex.pos[0], tex.pos[1]);
        glVertex3f(posx + (-0.0616 * zoom), posy + (0.035 * zoom), 0.00); // øverst venst
        glTexCoord2f(tex.pos[2], tex.pos[3]);
        glVertex3f(posx + (0.0616 * zoom), posy + (0.035 * zoom), 0.00); // øverst højre
        glTexCoord2f(tex.pos[4], tex.pos[5]);
        glVertex3f(posx + (0.0616 * zoom), posy + (-0.035 * zoom), 0.00); // nederst højre
        glTexCoord2f(tex.pos[6], tex.pos[7]);
        glVertex3f(posx + (-0.0616 * zoom), posy + (-0.035 * zoom), 0.00); // nederst venstre
        glEnd();
    }

    void growExplosive(brick bricks[]) {
        if (type != 'B' || justBecomeExplosive) {
            return;
        }

        if (bricknum > 0) {
            if (nbrick[row][bricknum - 1] != -1) {
                makeExplosive(bricks[nbrick[row][bricknum - 1]]);
            }
        }

        if (bricknum < 25) {
            if (nbrick[row][bricknum + 1] != -1) {
                makeExplosive(bricks[nbrick[row][bricknum + 1]]);
            }
        }

        if (row > 0) {
            if (nbrick[row - 1][bricknum] != -1) {
                makeExplosive(bricks[nbrick[row - 1][bricknum]]);
            }
        }

        if (row < 22) {
            if (nbrick[row + 1][bricknum] != -1) {
                makeExplosive(bricks[nbrick[row + 1][bricknum]]);
            }
        }

        if (row > 0 && bricknum > 0) {
            if (nbrick[row - 1][bricknum - 1] != -1) {
                makeExplosive(bricks[nbrick[row - 1][bricknum - 1]]);
            }
        }
        if (row > 0 && bricknum < 25) {
            if (nbrick[row - 1][bricknum + 1] != -1) {
                makeExplosive(bricks[nbrick[row - 1][bricknum + 1]]);
            }
        }

        if (row < 22 && bricknum > 0) {
            if (nbrick[row + 1][bricknum - 1] != -1) {
                makeExplosive(bricks[nbrick[row + 1][bricknum - 1]]);
            }
        }

        if (row < 22 && bricknum < 25) {
            if (nbrick[row + 1][bricknum + 1] != -1) {
                makeExplosive(bricks[nbrick[row + 1][bricknum + 1]]);
            }
        }
    }

    void breakable() {
        if (type == '3') {
            score = 300;
            hitsLeft = 1;
            type = '1'; //hehe..
            tex.frame = 2;
            tex.play();
        } else if (type == '4') {
            hitsLeft = 1;
            tex.frame = 2;
            tex.play();
        } else if (type == '9') {
            hitsLeft = 1;
            tex.frame = 3;
            tex.play();
        }
    }
};

void makeExplosive(brick &b) {
    if (b.type != 'B') {
        b.type = 'B';
        b.tex = *texExplosiveBrick;
        //NOTE: for some reason, the color of the object was changed, why??
        b.justBecomeExplosive = true;
    }
}

#include "loadlevel.cpp"
#include "effects.cpp"
#include "background.cpp"

void spawnpowerup(char powerup, pos a, pos b);

class bulletsClass {
    moving_object bullets[16];

public:
    int active;

    bulletsClass(const textureClass &texBullet) {
        for (int i = 0; i < 16; i++) {
            bullets[i].active = false;
            bullets[i].tex = texBullet;
            bullets[i].width = 0.02;
            bullets[i].height = 0.02;
        }
    }

    void shoot(const pos p) {
        //Find ledig bullet
        for (int i = 0; i < 16; i++) {
            if (!bullets[i].active) {
                soundMan.add(SND_SHOT, p.x);
                bullets[i].active = true;
                bullets[i].posx = p.x;
                bullets[i].posy = p.y;
                bullets[i].xvel = 0;
                bullets[i].yvel = 1.0;
                break;
            }
        }
    }

    void move() {
        for (int i = 0; i < 16; i++) {
            if (bullets[i].active) {
                //Flyt
                bullets[i].posy += bullets[i].yvel * globalMilliTicks;
            }
        }
    }

    void draw() {
        glColor4f(GL_WHITE);
        for (int i = 0; i < 16; i++) {
            if (bullets[i].active) {
                //draw

                bullets[i].tex.play();

                glLoadIdentity();
                glTranslatef(bullets[i].posx, bullets[i].posy, 0.0);

                glBindTexture(GL_TEXTURE_2D, bullets[i].tex.prop.texture);
                glBegin(GL_QUADS);
                glTexCoord2f(bullets[i].tex.pos[0], bullets[i].tex.pos[1]);
                glVertex3f(-bullets[i].width, bullets[i].height, 0.0);
                glTexCoord2f(bullets[i].tex.pos[2], bullets[i].tex.pos[3]);
                glVertex3f(bullets[i].width, bullets[i].height, 0.0);
                glTexCoord2f(bullets[i].tex.pos[4], bullets[i].tex.pos[5]);
                glVertex3f(bullets[i].width, -bullets[i].height, 0.0);
                glTexCoord2f(bullets[i].tex.pos[6], bullets[i].tex.pos[7]);
                glVertex3f(-bullets[i].width, -bullets[i].height, 0.0);
                glEnd();
            }
        }
    }

    void clear() {
        for (int i = 0; i < 16; i++) {
            bullets[i].active = false;
        }
    }

    void coldet(brick &b, effectManager &fxMan) {
        pos v;
        v.x = 0;
        v.y = bullets[0].xvel;

        for (int i = 0; i < 16; i++) {
            if (bullets[i].active) {
                //y
                if (bullets[i].posy + bullets[i].height / 10.0 > b.posy - b.height && bullets[i].posy + bullets[i].
                    height / 10.0 < b.posy + b.height) {
                    bool hit = false;
                    pos p;
                    p.x = b.posx;
                    p.y = b.posy;
                    //Venstre side:
                    if (bullets[i].posx > b.posx - b.width && bullets[i].posx < b.posx + b.width) {
                        hit = 1;
                    }

                    if (hit) {
                        b.hit(fxMan, p, v, 1);

                        bullets[i].active = false;

                        p.x = bullets[i].posx;
                        p.y = bullets[i].posy;

                        if (setting.eyeCandy) {
                            fxMan.set(FX_VAR_TYPE, FX_SPARKS);
                            fxMan.set(FX_VAR_COLDET, 1);
                            fxMan.set(FX_VAR_LIFE, 1300);
                            fxMan.set(FX_VAR_NUM, 16);
                            fxMan.set(FX_VAR_SIZE, 0.015f);
                            fxMan.set(FX_VAR_SPEED, 0.4f);
                            fxMan.set(FX_VAR_GRAVITY, 1.0f);

                            fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
                            fxMan.spawn(p);
                            fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
                            fxMan.spawn(p);
                            fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
                            fxMan.spawn(p);
                            fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
                            fxMan.spawn(p);
                        }
                    }
                } else if (bullets[i].posy > 1.6) {
                    bullets[i].active = false;
                }
            }
        }
    }
};

void brick::hit(effectManager &fxMan, pos poSpawnPos, pos poSpawnVel, bool ballHitMe) {
    pos p, s;

    if (type != '3' || player.powerup[PO_THRU])
        hitsLeft--;

    //We don't want to play a sound if this brick is not an explosive, and was hit by an explosion
    if (ballHitMe || type == 'B') {
        if (type == '3') //cement
        {
            soundMan.add(SND_CEMENT_BRICK_HIT, posx);
        } else if (type == '4' || type == '9') //glass or invisible
        {
            if (hitsLeft == 2) {
                soundMan.add(SND_INVISIBLE_BRICK_APPEAR, posx);
            } else if (hitsLeft == 1) {
                soundMan.add(SND_GLASS_BRICK_HIT, posx);
            } else {
                soundMan.add(SND_GLASS_BRICK_BREAK, posx);
            }
        } else if (type == 'B') //explosive
        {
            soundMan.add(SND_EXPL_BRICK_BREAK, posx);
        } else if (type == 'C') //Doom brick
        {
            soundMan.add(SND_DOOM_BRICK_BREAK, posx);
        } else {
            //All the other bricks
            soundMan.add(SND_NORM_BRICK_BREAK, posx);
        }
    }


    if (type != '3' || player.powerup[PO_THRU]) {
        //Brick was hit, dont do anything
        if (isdyingnormally || isexploding) {
            return;
        }
        player.score += (brick::score * player.multiply) * var.averageBallSpeed; //Speed bonus

        if (hitsLeft < 1 || type == 'B') //Hvis brikken er explosiv kan den ikke have nogle hits tilbage
        {
            collide = 0;

            updated_nbrick[row][bricknum] = -1;
            var.bricksHit = 1;

            gVar.deadTime = 0;

            spawnpowerup(powerup, poSpawnPos, poSpawnVel);
            powerup = '0';

            if (setting.eyeCandy) {
                p.x = posx;
                p.y = posy;
                s.x = width * 2;
                s.y = height * 2;

                fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
                fxMan.set(FX_VAR_COLDET, 1);
                fxMan.set(FX_VAR_LIFE, 1300);
                fxMan.set(FX_VAR_NUM, 20);
                fxMan.set(FX_VAR_SIZE, 0.03f);
                fxMan.set(FX_VAR_SPEED, 0.6f);
                fxMan.set(FX_VAR_GRAVITY, 0.7f);

                fxMan.set(FX_VAR_RECTANGLE, s);

                fxMan.set(FX_VAR_COLOR, tex.prop.glParColorInfo[0], tex.prop.glParColorInfo[1],
                          tex.prop.glParColorInfo[2]);
                fxMan.spawn(p);
            }

            if (type == 'B') {
                isexploding = 1;

                if (setting.eyeCandy) {
                    p.x = posx;
                    p.y = posy;
                    fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
                    fxMan.set(FX_VAR_COLDET, 1);
                    fxMan.set(FX_VAR_LIFE, 1200);
                    fxMan.set(FX_VAR_NUM, 10);
                    fxMan.set(FX_VAR_SIZE, 0.08f);
                    fxMan.set(FX_VAR_SPEED, 0.4f);
                    fxMan.set(FX_VAR_GRAVITY, -1.3f);

                    fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
                    fxMan.spawn(p);
                    fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
                    fxMan.spawn(p);
                    fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
                    fxMan.spawn(p);
                    fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
                    fxMan.spawn(p);
                }
            } else {
                isdyingnormally = 1;
            }
        } else {
            //No hits left
            tex.frame++;
            tex.play();
        } //Hits left
    }
}

glAnnounceTextClass announce;

int LinesCross(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, GLfloat *linx,
               GLfloat *liny) {
    float d = (x1 - x0) * (y3 - y2) - (y1 - y0) * (x3 - x2);
    if (fabs(d) < 0.001f) { return -1; }
    float AB = ((y0 - y2) * (x3 - x2) - (x0 - x2) * (y3 - y2)) / d;
    if (AB > 0.0 && AB < 1.0) {
        float CD = ((y0 - y2) * (x1 - x0) - (x0 - x2) * (y1 - y0)) / d;
        if (CD > 0.0 && CD < 1.0) {
            *linx = x0 + AB * (x1 - x0);
            *liny = y0 + AB * (y1 - y0);
            return 1;
        }
    }
    return 0;
}

// Leaves a trail behind the ball
class tracer {
    GLfloat x[100], y[100]; // Position
    GLfloat r[100], g[100], b[100]; // Farbe
    GLfloat a[100]; // Alpha wird stärker reduziert je weiter vom Ball
    GLfloat s[100]; // Größe wird kleiner je weiter vom Ball
    GLfloat cr, cg, cb; // Aktuelle Farben
    bool active[100];
    int color;
    GLfloat lastX, lastY; // Letzte Position

public:
    GLfloat height, width;
    textureClass *tex;
    int len;

    // Constructor bleibt gleich
    tracer() {
        len = 100;
        lastX = lastY = 0;
        cr = 1;
        cg = cb = 0;
        height = width = 0.01;
        for (int i = 0; i < 100; i++) active[i] = false;
    }

    void draw() {
        for (int i = 0; i < len; i++) {
            if (active[i]) {
                // Stärkere Alpha-Reduktion für ältere Partikel
                a[i] -= (4.0f + i * 0.1f) * globalMilliTicksSinceLastDraw;

                // Partikel werden dünner mit der Zeit
                s[i] -= 2.0f * globalMilliTicksSinceLastDraw;
                if (s[i] < 0.2f) s[i] = 0.2f;

                if (a[i] < 0.0f) {
                    active[i] = false;
                    continue;
                }

                tex->play();
                glBindTexture(GL_TEXTURE_2D, tex->prop.texture);
                glLoadIdentity();
                glTranslatef(x[i], y[i], 0.0);

                // Alpha zusätzlich basierend auf Index reduzieren
                float indexBasedAlpha = a[i] * (1.0f - (float) i / len);
                glColor4f(r[i], g[i], b[i], indexBasedAlpha);

                glBegin(GL_QUADS);
                glTexCoord2f(tex->pos[0], tex->pos[1]);
                glVertex3f(-width * s[i], height * s[i], 0.00);
                glTexCoord2f(tex->pos[2], tex->pos[3]);
                glVertex3f(width * s[i], height * s[i], 0.00);
                glTexCoord2f(tex->pos[4], tex->pos[5]);
                glVertex3f(width * s[i], -height * s[i], 0.00);
                glTexCoord2f(tex->pos[6], tex->pos[7]);
                glVertex3f(-width * s[i], -height * s[i], 0.00);
                glEnd();
            }
        }
    }

    void colorRotate(const bool explosive, GLfloat c[]) {
        color++;
        if (color > 5)
            color = 0;

        if (!explosive) {
            cr = c[0];
            cg = c[1];
            cb = c[2];
        } else {
            cr = 1.0;
            cg = 0.6;
            cb = 0.0;
        }
    }

    void update(const GLfloat nx, const GLfloat ny) {
        // If long enough away
        const GLfloat dist = sqrt(pow(nx - lastX, 2) + pow(ny - lastY, 2));
        if (dist > 0.01) {
            lastX = nx;
            lastY = ny;
            //find a non-active trail-part
            for (int i = 0; i < len; i++) {
                if (!active[i]) {
                    active[i] = true;
                    a[i] = 1.0; //tweak me
                    x[i] = nx;
                    y[i] = ny;
                    s[i] = 1.0;
                    r[i] = cr;
                    g[i] = cg;
                    b[i] = cb;
                    break;
                }
            }
        }
    }
};

class ball : public moving_object {
    GLfloat rad;
    bool growing, shrinking;
    GLfloat destwidth, growspeed;

public:
    tracer tail;
    bool explosive; //Makes brick explosive (to get a explosion effect) and explode it

    bool glued; //Sidder vi fast på padden for øjeblikket?
    GLfloat gluedX;
    //Variabler med forududregnede værdier
    GLfloat bsin[32], bcos[32];

    bool aimdir;
    textureClass fireTex;

    GLfloat lastX, lastY;

    ball() {
        growing = false;
        growspeed = 0.1;
        width = 0.0;
        height = 0.0;
        glued = false;
        posx = 0.0f;
        posy = 0.0f;
        aimdir = false;
    }

    void hit(GLfloat c[]) {
        if (setting.eyeCandy)
            tail.colorRotate(explosive, c);
    }

    void move() {
        //vi laver lige den her coldet her...
        if (posx + width > 1.6 && xvel > 0.0) {
            soundMan.add(SND_BALL_HIT_BORDER, posx);
            xvel *= -1;
        } else if (posx - width < -1.6 && xvel < 0.0) {
            soundMan.add(SND_BALL_HIT_BORDER, posx);
            xvel *= -1;
        } else if (posy + width > 1.25 && yvel > 0.0) {
            soundMan.add(SND_BALL_HIT_BORDER, posx);
            yvel *= -1;
        } else if (posy - width < -1.24) {
            active = false;
        }

        posx += xvel * globalMilliTicks;

        if (!glued) {
            posy += yvel * globalMilliTicks;
        } else {
            gVar.deadTime = 0;
        }

        if (setting.eyeCandy)
            tail.update(posx, posy);
    }

    void draw(const paddle_class &paddle) {
        GLfloat newsize;

        if (setting.eyeCandy)
            tail.draw();

        if (growing) {
            newsize = growspeed * globalMilliTicksSinceLastDraw;
            width += newsize;
            height += newsize;

            if (width >= destwidth) {
                width = destwidth;
                height = destwidth;

                growing = false;
            }

            tail.width = width;
            tail.height = height;
        } else if (shrinking) {
            newsize = growspeed * globalMilliTicksSinceLastDraw;
            width -= newsize;
            height -= newsize;
            if (width <= destwidth) {
                width = destwidth;
                height = destwidth;

                shrinking = false;
            }

            tail.width = width;
            tail.height = height;
        }

        if (glued && player.powerup[PO_LASER]) {
            if (player.powerup[PO_AIM]) {
                if (aimdir == 0) {
                    rad -= 1.2f * globalMilliTicksSinceLastDraw;

                    if (rad < BALL_MIN_DEGREE)
                        aimdir = true;
                } else {
                    rad += 1.2f * globalMilliTicksSinceLastDraw;
                    if (rad > BALL_MAX_DEGREE + BALL_MIN_DEGREE)
                        aimdir = false;
                }
                setangle(rad);
            } else {
                getRad();
            }

            const GLfloat bxb = cos(rad) * 0.5f;
            const GLfloat byb = sin(rad) * 0.5f;

            glLoadIdentity();
            glTranslatef(posx, posy, 0.0f);
            glDisable(GL_TEXTURE_2D);
            glLineWidth(1.0);
            glEnable(GL_LINE_SMOOTH);
            glBegin(GL_LINES);
            glColor4f(random_float(2, 0), random_float(1, 0), 0.0, 0.0); // ???
            glVertex3f(0.0, 0.0, 0.0);
            glColor4f(random_float(2, 0), 0.0, 0.0, 1.0);
            glVertex3f(bxb, byb, 0.0);
            glEnd();

            glPointSize(5.0f);
            glColor4f(GL_FULL_RED);
            glEnable(GL_POINT_SMOOTH);
            glBegin(GL_POINTS);
            glVertex3f(bxb, byb, 0.0);
            glEnd();
        }

        if (!glued && player.powerup[PO_AIMHELP]) {
            //Use line intersect to determine if this ball will collide with the paddle

            getRad();
            GLfloat p[4], b[4], o[2]; //Paddle line, ball line, bounceoff endpoint
            p[0] = paddle.posx - paddle.width;
            p[1] = paddle.posx + paddle.width;

            p[2] = paddle.posy + paddle.height + height;
            p[3] = paddle.posy + paddle.height + height;

            b[0] = posx;
            b[1] = posx + (cos(rad) * 3.0);
            b[2] = posy;
            b[3] = posy + (sin(rad) * 3.0);

            GLfloat cx, cy;
            if (LinesCross(p[0], p[2], p[1], p[3], b[0], b[2], b[1], b[3], &cx, &cy)) {
                const GLfloat R = bounceOffAngle(paddle.width, paddle.posx, cx);
                o[0] = cx + (cos(R) * 2.0);
                o[1] = cy + (sin(R) * 2.0);
                glLoadIdentity();
                glTranslatef(0.0, 0.0, 0.0);
                glDisable(GL_TEXTURE_2D);
                glLineWidth(2.0);
                glEnable(GL_LINE_SMOOTH);
                glBegin(GL_LINE_STRIP);
                //Line from ball to paddle
                glColor4f(1.0, 0.0, 0.0, 0.0); // ???
                glVertex3f(b[0], b[2], 0.0);
                glColor4f(1.0, 1.0, 0.0, 1.0);
                glVertex3f(cx, cy, 0.0);
                //Bounce off line.
                glColor4f(1.0, 0.0, 0.0, 0.0); // ???
                glVertex3f(o[0], o[1], 0.0);
                glEnd();
            }
        }

        glLoadIdentity();
        glTranslatef(posx, posy, 0.0);
        glEnable(GL_TEXTURE_2D);

        glColor4f(GL_WHITE);

        if (explosive) {
            fireTex.play();
            glBindTexture(GL_TEXTURE_2D, fireTex.prop.texture);
            glColor4f(fireTex.prop.glTexColorInfo[0], fireTex.prop.glTexColorInfo[1], fireTex.prop.glTexColorInfo[2],
                      fireTex.prop.glTexColorInfo[3]);

            glBegin(GL_QUADS);
            glTexCoord2f(fireTex.pos[0], fireTex.pos[1]);
            glVertex3f(-width, height, 0.0);
            glTexCoord2f(fireTex.pos[2], fireTex.pos[3]);
            glVertex3f(width, height, 0.0);
            glTexCoord2f(fireTex.pos[4], fireTex.pos[5]);
            glVertex3f(width, -height, 0.0);
            glTexCoord2f(fireTex.pos[6], fireTex.pos[7]);
            glVertex3f(-width, -height, 0.0);
            glEnd();
        } else {
            tex.play();
            glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
            glColor4f(tex.prop.glTexColorInfo[0], tex.prop.glTexColorInfo[1], tex.prop.glTexColorInfo[2],
                      tex.prop.glTexColorInfo[3]);

            glBegin(GL_QUADS);
            glTexCoord2f(tex.pos[0], tex.pos[1]);
            glVertex3f(-width, height, 0.0);
            glTexCoord2f(tex.pos[2], tex.pos[3]);
            glVertex3f(width, height, 0.0);
            glTexCoord2f(tex.pos[4], tex.pos[5]);
            glVertex3f(width, -height, 0.0);
            glTexCoord2f(tex.pos[6], tex.pos[7]);
            glVertex3f(-width, -height, 0.0);
            glEnd();
        }

#ifdef DEBUG_DRAW_BALL_QUAD
      glLoadIdentity();
      glTranslatef(posx, posy, 0.0);
      glDisable( GL_TEXTURE_2D );
      glColor4f(GL_WHITE);
      glBegin( GL_LINES );
        glVertex3f( -width, height, 0);
        glVertex3f( width, height, 0);

        glVertex3f( -width, -height, 0);
        glVertex3f( width, -height,0);

        glVertex3f( -width, height, 0);
        glVertex3f( -width, -height, 0);

        glVertex3f( width, height, 0);
        glVertex3f( width, -height, 0 );

      glEnd();
      glEnable(GL_TEXTURE_2D);
#endif
    }

    GLfloat getRad() {
        rad = atan2(yvel, xvel);
        return (rad);
    }

    void setangle(GLfloat o) {
        if (o < BALL_MIN_DEGREE) {
            o = BALL_MIN_DEGREE;
        }

        if (o > BALL_MAX_DEGREE + BALL_MIN_DEGREE) {
            o = BALL_MAX_DEGREE + BALL_MIN_DEGREE;
        }

        rad = o;
        xvel = velocity * cos(rad);
        yvel = velocity * sin(rad);
    }

    void setspeed(GLfloat v) {
        if (v > difficulty.maxballspeed[player.difficulty]) {
            velocity = difficulty.maxballspeed[player.difficulty];
        } else {
            velocity = v;
        }

        getRad();
        xvel = velocity * cos(rad);
        yvel = velocity * sin(rad);
    }

    void setSize(GLfloat s) {
        float rad;

        if (s > width)
            growing = 1;
        else if (s < width)
            shrinking = 1;

        destwidth = s;

        int i = 0;

        //opdater points
        for (rad = 0.0; rad < 6.3; rad += 0.2) {
            if (i < 32) {
                bsin[i] = sin(rad) * s;
                bcos[i] = cos(rad) * s;
            }
            i++;
        }
    }
};

void collision_ball_brick(brick &br, ball &ba, pos &p, effectManager &fxMan);

void padcoldet(ball &b, paddle_class &p, pos &po);

#define MAXBALLS 16

class ballManager {
public:
    int activeBalls;
    ball b[MAXBALLS];
    textureClass tex[3];

    void initBalls() {
        activeBalls = 0;
        clear();
    }

    ballManager(textureClass btex[]) {
        tex[0] = btex[0];
        tex[1] = btex[1];
        tex[2] = btex[2];


        for (int i = 0; i < MAXBALLS; i++) {
            b[i].tex = tex[0];
            b[i].fireTex = tex[1];
            b[i].tail.tex = &tex[2];
        }

        initBalls();
    }

    void getSpeed() {
        var.averageBallSpeed = 0.0;
        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                var.averageBallSpeed += b[i].velocity;
            }
        }
        var.averageBallSpeed /= activeBalls;
    }

    //klon alle aktive bolde
    void multiply() {
        int a = 0, c = 0;
        int i;
        //How many balls are active?
        for (i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                a++;
            }
        }

        for (i = 0; i < MAXBALLS; i++) {
            if (b[i].active && c != a) {
                pos op;
                c++;
                op.y = b[i].posy;
                op.x = b[i].posx;
                spawn(op, 0, 0.0f, b[i].velocity, random_float(BALL_MAX_DEGREE + BALL_MIN_DEGREE, 0));
            }
        }
    }

    void unglue() {
        for (int i = 0; i < MAXBALLS; i++) {
            b[i].glued = 0;
        }
    }

    void spawn(pos p, bool glued, GLfloat gx, GLfloat speed, GLfloat angle) {
        for (int i = 0; i < MAXBALLS; i++) {
            if (!b[i].active) {
                activeBalls++;
                b[i].tex = tex[0];
                b[i].fireTex = tex[1];
                b[i].glued = glued;

                b[i].width = 0.0;
                b[i].height = 0.0;
                b[i].gluedX = gx;
                b[i].active = 1;
                b[i].collide = 1;
                b[i].reflect = 1;
                b[i].lastX = p.x;
                b[i].lastY = p.y;
                b[i].posx = p.x;
                b[i].posy = p.y;
                b[i].explosive = 0;
                b[i].setspeed(speed);
                b[i].setangle(angle);
                b[i].setSize(0.025);

                //New balls get already applied powerups if not hard
                if (player.difficulty < HARD) {
                    b[i].explosive = player.powerup[PO_EXPLOSIVE];

                    if (player.powerup[PO_SMALLBALL]) {
                        powerup(PO_SMALLBALL);
                    }

                    if (player.powerup[PO_BIGBALL]) {
                        powerup(PO_BIGBALL);
                    }
                }
                getSpeed();
                break;
            }
        }
    }

    void clear() {
        activeBalls = 0;
        for (int i = 0; i < MAXBALLS; i++) {
            b[i].active = 0;
        }
        getSpeed();
    }

    void move() {
        int a = 0;

        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                b[i].move();
                a++;
            }
        }
        activeBalls = a;
    }

    void draw(paddle_class &paddle) {
        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                b[i].draw(paddle);
            }
        }
    }

    void bcoldet(brick &bri, effectManager &fxMan) {
        pos p;
        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                p.x = 100;
                collision_ball_brick(bri, b[i], p, fxMan);
                if (p.x < 50) //we totally hit?? :P
                {
                    getSpeed();

                    if (setting.eyeCandy) {
                        //spawn partikler
                        fxMan.set(FX_VAR_TYPE, FX_SPARKS);
                        fxMan.set(FX_VAR_COLDET, 1);

                        fxMan.set(FX_VAR_SPEED, 1.0f);

                        fxMan.set(FX_VAR_LIFE, 1500);
                        fxMan.set(FX_VAR_NUM, 16);
                        fxMan.set(FX_VAR_SIZE, 0.015f);

                        fxMan.set(FX_VAR_COLOR, 1.0, 1.0, 0.8);

                        fxMan.spawn(p);
                    }
                }
            }
        }
    }

    int pcoldet(paddle_class &paddle, effectManager &fxMan) {
        int hits = 0;
        pos p;
        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                if (b[i].glued)
                    b[i].posx = paddle.posx + paddle.width - b[i].gluedX;

                p.x = 100;
                padcoldet(b[i], paddle, p);
                if (p.x < 50) {
                    hits++;
                    getSpeed();

                    if (player.powerup[PO_GLUE]) {
                        soundMan.add(SND_GLUE_BALL_HIT_PADDLE, p.x);
                    } else {
                        soundMan.add(SND_BALL_HIT_PADDLE, p.x);
                    }


                    if (setting.eyeCandy) {
                        //spawn partikler
                        fxMan.set(FX_VAR_TYPE, FX_SPARKS);
                        fxMan.set(FX_VAR_LIFE, 2000);
                        fxMan.set(FX_VAR_GRAVITY, 0.6f);
                        fxMan.set(FX_VAR_NUM, 16);
                        fxMan.set(FX_VAR_COLDET, 1);
                        fxMan.set(FX_VAR_SIZE, 0.01f);
                        fxMan.set(FX_VAR_COLOR, 1.0, 1.0, 0.8);
                        p.y = paddle.posy + paddle.height;
                        fxMan.set(FX_VAR_SPEED, 0.5f);
                        fxMan.spawn(p);
                    } //eyecandy
                } // if col
            } //if active
        } //for loop
        return (hits);
    } //pcoldet

    void updatelast() {
        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                b[i].lastX = b[i].posx;
                b[i].lastY = b[i].posy;
            }
        }
    }

    void powerup(const int powerup) {
        for (int i = 0; i < MAXBALLS; i++) {
            if (b[i].active) {
                switch (powerup) {
                    case PO_BIGBALL: //big balls
                        b[i].setSize(0.04);
                        b[i].setspeed(difficulty.ballspeed[player.difficulty]);
                        break;
                    case PO_SMALLBALL: //small balls
                        b[i].setSize(0.015);
                        //speed bolden op
                        b[i].setspeed(
                            b[i].velocity + ((b[i].velocity / 100.f) * difficulty.speedup[player.difficulty]));
                        break;
                    case PO_NORMALBALL: // normal balls
                        b[i].setSize(0.025);
                        b[i].setspeed(difficulty.ballspeed[player.difficulty]);
                        break;
                    case PO_EXPLOSIVE: //exploderer brikker
                        b[i].explosive = true;
                        b[i].tail.colorRotate(true, nullptr);
                        break;
                    default: ;
                }
            }
        }

        getSpeed();
    }
};

float bounceOffAngle(GLfloat width, GLfloat posx, GLfloat hitx) {
    return ((BALL_MAX_DEGREE / (width * 2.0)) * (posx + width - hitx) + BALL_MIN_DEGREE);
}

class powerupClass : public moving_object {
public:
    int score;
    int type;
    int level, maxlevel;
    GLfloat gravity;

    powerupClass() {
        posx = 0.0;
        posy = 0.0;
        xvel = 0.0;
        yvel = 0.0;
        width = 0.055;
        height = 0.055;
    }

    void move() {
        //grav
        yvel -= gravity * globalMilliTicks;
        //SDL_Log("%s", yvel);
        posx += xvel * globalMilliTicks;
        posy += yvel * globalMilliTicks;
    }


    bool coldet(paddle_class &p, effectManager &fxMan, ballManager &bMan) {
        bool col = 0;
        if (posx + width > 1.6 && xvel > 0.0) {
            col = 1;
            xvel *= -1;
        } else if (posx - width < -1.6 && xvel < 0.0) {
            col = 1;
            xvel *= -1;
        } else if (posy + width > 1.25 && yvel > 0.0) {
            col = 1;
            yvel *= -1;
        } else if (posy - width < -1.24) {
            active = 0;
        }

        if (col) {
            soundMan.add(SND_PO_HIT_BORDER, posx);
        }

        //idiotisk lavet...

        bool ycol = 0;
        bool xcol = 0;
        pos fxpos, fxSize;

        //En side
        if (posx + width > p.posx - p.width && posx + width < p.posx + p.width) {
            xcol = 1;
        }

        if (posx - width > p.posx - p.width && posx - width < p.posx + p.width) {
            xcol = 1;
        }

        if (posy - height < p.posy + p.height && posy - height > p.posy - p.height) {
            ycol = 1;
        }

        if (posy + height < p.posy + p.height && posy + height > p.posy - p.height) {
            ycol = 1;
        }

        if (xcol && ycol) {
            if (setting.eyeCandy) {
                fxpos.x = posx;
                fxpos.y = posy;
                fxSize.x = width;
                fxSize.y = height;

                fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
                fxMan.set(FX_VAR_COLDET, 1);
                fxMan.set(FX_VAR_SPEED, yvel / 1.5f);
                fxMan.set(FX_VAR_LIFE, 1500);
                fxMan.set(FX_VAR_GRAVITY, 0.7f);
                fxMan.set(FX_VAR_NUM, 20);
                fxMan.set(FX_VAR_SIZE, 0.03f);

                fxMan.set(FX_VAR_RECTANGLE, fxSize);

                fxMan.set(FX_VAR_COLOR, tex.prop.glParColorInfo[0], tex.prop.glParColorInfo[1],
                          tex.prop.glParColorInfo[2]);
                fxMan.spawn(fxpos);
            }
            active = 0;

            //Score
            player.score += score * player.multiply;
            //Apply powerup:
            switch (type) {
                case PO_COIN:
                    player.coins += 1000;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_GLUE:
                    player.coins += 150;
                    player.powerup[PO_GLUE] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_BIGBALL:
                    player.coins += 30;
                    bMan.powerup(PO_BIGBALL);
                    player.powerup[PO_BIGBALL] = 1;
                    player.powerup[PO_NORMALBALL] = 0;
                    player.powerup[PO_SMALLBALL] = 0;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_NORMALBALL:
                    player.coins += 50;
                    bMan.powerup(PO_NORMALBALL);
                    player.powerup[PO_NORMALBALL] = 1;
                    player.powerup[PO_BIGBALL] = 0;
                    player.powerup[PO_SMALLBALL] = 0;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_SMALLBALL:
                    player.coins += 10;
                    bMan.powerup(PO_SMALLBALL);
                    player.powerup[PO_SMALLBALL] = 1;
                    player.powerup[PO_BIGBALL] = 0;
                    player.powerup[PO_NORMALBALL] = 0;
                    soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
                    break;
                case PO_MULTIBALL:
                    player.coins += 100;
                    bMan.multiply();
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_AIM:
                    player.coins += 50;
                    if (player.difficulty == 0) {
                        player.powerup[PO_GLUE] = 1;
                    }
                    if (!player.powerup[PO_AIM]) {
                        player.powerup[PO_AIM] = 1;
                        player.powerup[PO_LASER] = 1;
                    } else {
                        player.powerup[PO_GLUE] = 1;
                    }
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_GROWPADDLE:
                    player.coins += 100;
                    if (p.width < 0.4) p.grow(p.width + 0.03);
                    player.powerup[PO_GUN] = 0;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_SHRINKPADDLE:
                    player.coins += 10;
                    if (p.width > 0.02) p.grow(p.width - 0.02);
                    player.powerup[PO_GUN] = 0;
                    soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
                    break;
                case PO_EXPLOSIVE:
                    player.coins += 150;
                    bMan.powerup(PO_EXPLOSIVE);
                    player.powerup[PO_EXPLOSIVE] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_GUN:
                    player.coins += 200;
                    player.powerup[PO_GUN] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_THRU:
                    player.coins += 300;
                    player.powerup[PO_THRU] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_LASER:
                    player.coins += 40;
                    player.powerup[PO_LASER] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_LIFE:
                    player.coins += 400;
                    player.lives++;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_DIE:
                    player.coins += 1;
                    player.explodePaddle = 1;
                    player.powerup[PO_DIE] = 1;
                    //NOTE: no sound here, SND_DIE is played when paddle dissapers
                    break;
                case PO_DROP:
                    player.coins += 1;
                    player.powerup[PO_DROP] = 1;
                    soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
                    break;
                case PO_DETONATE:
                    player.coins += 200;
                    player.powerup[PO_DETONATE] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_EXPLOSIVE_GROW:
                    player.coins += 100;
                    player.powerup[PO_EXPLOSIVE_GROW] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_EASYBRICK:
                    player.coins += 90;
                    player.powerup[PO_EASYBRICK] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                case PO_NEXTLEVEL:
                    player.coins += 100;
                    player.powerup[PO_NEXTLEVEL] = 1;
                    //NOTE: no sound here, SND_NEXTLEVEL is played when changing level
                    break;
                case PO_AIMHELP:
                    player.coins += 50;
                    player.powerup[PO_AIMHELP] = 1;
                    soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
                    break;
                default: ;
            }
            return true;
        }

        return false;
    }

    void die(effectManager &fxMan) {
        active = 0;
        if (setting.eyeCandy) {
            struct pos p;
            p.x = posx;
            p.y = posy;
            fxMan.set(FX_VAR_TYPE, FX_SPARKS);
            fxMan.set(FX_VAR_COLDET, 1);
            fxMan.set(FX_VAR_LIFE, 1250);
            fxMan.set(FX_VAR_NUM, 16);

            fxMan.set(FX_VAR_SPEED, 0.8f);
            fxMan.set(FX_VAR_GRAVITY, 0.6f);
            fxMan.set(FX_VAR_SIZE, 0.025f);
            fxMan.set(FX_VAR_COLOR, tex.prop.glParColorInfo[0], tex.prop.glParColorInfo[1], tex.prop.glParColorInfo[2]);
            fxMan.spawn(p);

            fxMan.set(FX_VAR_SPEED, 0.4f);
            fxMan.set(FX_VAR_SIZE, 0.05f);
            fxMan.set(FX_VAR_GRAVITY, -1.0f);
            fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
            fxMan.spawn(p);
        }
    }

    void draw() {
        tex.play();

        glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
        glColor4f(tex.prop.glTexColorInfo[0], tex.prop.glTexColorInfo[1], tex.prop.glTexColorInfo[2],
                  tex.prop.glTexColorInfo[3]);
        glLoadIdentity();
        glTranslatef(posx, posy, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(tex.pos[0], tex.pos[1]);
        glVertex3f(-width, height, 0.00); // øverst venst
        glTexCoord2f(tex.pos[2], tex.pos[3]);
        glVertex3f(width, height, 0.00); // øverst højre
        glTexCoord2f(tex.pos[4], tex.pos[5]);
        glVertex3f(width, -height, 0.00); // nederst højre
        glTexCoord2f(tex.pos[6], tex.pos[7]);
        glVertex3f(-width, -height, 0.00); // nederst venstre
        glEnd();
    }
};

#define MAXPOWERUPS 64

class powerupManager {
private:
    int i;
    powerupClass p[MAXPOWERUPS];
    textureClass *tex;

public:
    void init(textureClass texPowerup[]) {
        tex = new textureClass[40];
        tex = texPowerup;
    }

    powerupManager() {
        clear();
    }

    void clear() {
        for (i = 0; i < MAXPOWERUPS; i++) {
            p[i].active = 0;
        }
    }

    void die(effectManager &fxMan) {
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (p[i].active) {
                p[i].die(fxMan);
            }
        }
    }

    void spawn(pos spawnpos, pos velocity, int type) {
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (!p[i].active) {
                p[i].gravity = 0.7;
                p[i].type = type;
                p[i].posx = spawnpos.x;
                p[i].posy = spawnpos.y;
                p[i].xvel = velocity.x * -1;
                p[i].yvel = velocity.y * -1;
                p[i].active = 1;

                //Give texture that this type has.
                p[i].tex = tex[type];

                //FIXME: rewrite as a switch
                //Set colors and score
                if (type == PO_GLUE) {
                    p[i].score = 500;
                }


                if (type == PO_MULTIBALL) {
                    p[i].score = 500;
                }

                if (type == PO_BIGBALL) {
                    p[i].score = 300;
                }

                if (type == PO_NORMALBALL) {
                    p[i].score = 400;
                }

                if (type == PO_SMALLBALL) {
                    p[i].score = 100;
                }

                if (type == PO_AIM) {
                    p[i].score = 1600;
                }

                if (type == PO_GROWPADDLE) {
                    p[i].score = 500;
                }

                if (type == PO_SHRINKPADDLE) {
                    p[i].score = -1000;
                }

                if (type == PO_EXPLOSIVE) {
                    p[i].score = 1400;
                }

                if (type == PO_GUN) {
                    p[i].score = 1800;
                }

                if (type == PO_THRU) {
                    p[i].score = 1000;
                }

                if (type == PO_LASER) {
                    p[i].score = 500;
                }

                if (type == PO_LIFE) {
                    p[i].score = 1000;
                }

                if (type == PO_DIE) {
                    p[i].score = -1000;
                }

                if (type == PO_DROP) {
                    p[i].score = -1000;
                }

                if (type == PO_DETONATE) {
                    p[i].score = 1000;
                }

                if (type == PO_EXPLOSIVE_GROW) {
                    p[i].score = 1000;
                }

                if (type == PO_EASYBRICK) {
                    p[i].score = 1000;
                }

                if (type == PO_NEXTLEVEL) {
                    p[i].score = 1000;
                }

                if (type == PO_AIMHELP) {
                    p[i].score = 1000;
                }

                break; //Whats this doing?
            }
        }
    }

    int coldet(paddle_class &paddle, effectManager &fxMan, ballManager &bMan) {
        int hits = 0;
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (p[i].active) {
                if (p[i].coldet(paddle, fxMan, bMan)) {
                    hits++;
                }
            }
        }
        return (hits);
    }

    void move() {
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (p[i].active) {
                p[i].move();
            }
        }
    }

    void draw() {
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (p[i].active) {
                p[i].draw();
            }
        }
    }
};

powerupManager pMan;

void spawnpowerup(char powerup, pos a, pos b) {
    if (powerup == '1') {
        pMan.spawn(a, b,PO_GROWPADDLE);
    }

    if (powerup == '2') {
        pMan.spawn(a, b,PO_SHRINKPADDLE);
    }

    if (powerup == '3') {
        pMan.spawn(a, b,PO_DIE);
    }

    if (powerup == '4') {
        pMan.spawn(a, b,PO_GLUE);
    }

    if (powerup == 'A') {
        pMan.spawn(a, b,PO_EASYBRICK);
    }

    if (powerup == 'B') {
        pMan.spawn(a, b,PO_EXPLOSIVE);
    }

    if (powerup == 'C') {
        pMan.spawn(a, b,PO_NEXTLEVEL);
    }

    if (powerup == 'D') {
        pMan.spawn(a, b,PO_AIMHELP);
    }

    if (powerup == 'E') {
        pMan.spawn(a, b,PO_COIN);
    }

    if (powerup == '5') {
        pMan.spawn(a, b,PO_MULTIBALL);
    }

    if (powerup == '6') {
        pMan.spawn(a, b,PO_THRU);
    }

    if (powerup == '7') {
        pMan.spawn(a, b,PO_DROP);
    }

    if (powerup == '8') {
        pMan.spawn(a, b,PO_DETONATE);
    }

    if (powerup == '9') {
        pMan.spawn(a, b,PO_EXPLOSIVE_GROW);
    }

    if (powerup == 'F') {
        pMan.spawn(a, b,PO_BIGBALL);
    }

    if (powerup == 'G') {
        pMan.spawn(a, b,PO_NORMALBALL);
    }

    if (powerup == 'H') {
        pMan.spawn(a, b,PO_SMALLBALL);
    }

    if (powerup == 'I') {
        pMan.spawn(a, b,PO_AIM);
    }

    if (powerup == 'P') {
        pMan.spawn(a, b,PO_GUN);
    }

    if (powerup == 'R') {
        pMan.spawn(a, b,PO_LASER);
    }

    if (powerup == 'O') {
        pMan.spawn(a, b,PO_LIFE);
    }
}

void initGL() {
    //     printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
    // printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
    //     printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
    //printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));

    /* Enable smooth shading */
    glShadeModel(GL_SMOOTH);

    /* Set the background black */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* Depth buffer setup */
    glClearDepth(1.0f);

    /* Enables Depth Testing */
    //  glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

float random_float(const float total, const float negative) {
    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution dist(-negative, total - negative);
    return dist(rng);
}

void resetPlayerPowerups() {
    for (bool &i: player.powerup) {
        i = false;
    }
}

void initNewGame() {
    player.level = 0;
    player.score = 0;
    gVar.deadTime = 0;

    gVar.newLevel = true;
    gVar.newLife = true;

    player.multiply = 1;

    switch (player.difficulty) {
        case EASY:
            player.coins = 600;
            player.lives = 5;
            break;
        case NORMAL:
            player.coins = 300;
            player.lives = 3;
            break;
        case HARD:
            player.coins = 0;
            player.lives = 3;
            break;
        default: ;
    }

    resetPlayerPowerups();
}

void pauseGame() {
    var.paused = true;
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void resumeGame() {
#ifndef DEBUG_NO_RELATIVE_MOUSE
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    var.paused = false;
    var.menu = 0;
}

void createPlayfieldBorder(GLuint *dl, const textureClass &tex) {
    *dl = glGenLists(1);
    glNewList(*dl,GL_COMPILE);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, 0.0);

    // top
    glColor4f(GL_WHITE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
    glBegin(GL_POINTS);
    glVertex3f(-1.0f, 1.0f, 0.0);
    glEnd();

    glBegin(GL_QUADS);

    const float width = 0.06f;
    // left
    // linke Säule
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f + width, 1.0f, 0.0f);
    glTexCoord2f(1.0f, -1.0f);
    glVertex3f(-1.0f + width, -1.0f, 0.0f);
    glTexCoord2f(0.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    // right
    // rechte Säule
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f - width, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, -1.0f);
    glVertex3f(1.0f - width, -1.0f, 0.0f);
    glEnd();
    glEndList();
}

void collision_ball_brick(brick &br, ball &ba, pos &p, effectManager &fxMan) {
    GLfloat x, y;
    int i = 0;

    int points = 0;
    GLfloat px = 0, py = 0;
    bool col = 0;
    bool dirfound = 0;
    GLfloat dist[4] = {4.0, 4.0, 4.0, 4.0};
    //measure the distance from last pos to each possible impact, the shortest should be the right one

    //vi tager y først da der er mindst brikker
    if (ba.posy < br.posy + br.height + ba.height && ba.posy > br.posy - br.height - ba.height) {
        //SDL_Log(" y ");
        if (ba.posx > br.posx - br.width - ba.width && ba.posx < br.posx + br.width + ba.width) {
            //SDL_Log(" x ");
            for (i = 0; i < 32; i++) // 32 punkter præcis
            {
                x = ba.bsin[i];
                y = ba.bcos[i];

                if (ba.posx + x >= br.posx - br.width && ba.posx + x <= br.posx + br.width) {
                    if (ba.posy + y <= br.posy + br.height && ba.posy + y >= br.posy - br.height) {
                        //Vi har helt sikkert ramt
                        points++;
                        px += x;
                        py += y;
                        col = 1;
                    } //y
                } //x
            } //32 punkters for loop

            if (col) {
                px /= points;
                py /= points;

                if (ba.lastX - px <= br.posx - br.width && !br.n(0)) //
                {
                    dirfound = 1;
                    //    SDL_Log("På venstre");
                    dist[0] = sqrt(
                        pow(br.posx - br.width - (ba.lastX + px), 2) + pow((ba.posy + py) - (ba.lastY + py), 2));
                }

                if (ba.lastX - px >= br.posx + br.width && !br.n(1)) {
                    dirfound = 1;
                    // SDL_Log("På højre");
                    dist[1] = sqrt(
                        pow(br.posx + br.width - (ba.lastX + px), 2) + pow((ba.posy + py) - (ba.lastY + py), 2));
                }

                if (ba.lastY - py <= br.posy - br.height && !br.n(3)) {
                    dirfound = 1;
                    // SDL_Log("På bunden");
                    dist[2] = sqrt(
                        pow((ba.posx + px) - (ba.lastX + px), 2) + pow(br.posy - br.height - (ba.lastY + py), 2));
                }

                if (ba.lastY - py >= br.posy + br.height && !br.n(2)) // &&
                {
                    dirfound = 1;
                    // SDL_Log("På toppen");
                    dist[3] = sqrt(
                        pow((ba.posx + px) - (ba.lastX + px), 2) + pow(br.posy + br.height - (ba.lastY + py), 2));
                }


                //Was hit on left
                if (dist[0] < dist[1] && dist[0] < dist[2] && dist[0] < dist[3]) {
                    ba.posx = br.posx - br.width - ba.width;
                    if (ba.xvel > 0.0 && !player.powerup[PO_THRU])
                        ba.xvel *= -1;
                }

                //Was hit on right
                if (dist[1] < dist[0] && dist[1] < dist[2] && dist[1] < dist[3]) {
                    ba.posx = br.posx + br.width + ba.width;
                    if (ba.xvel < 0 && !player.powerup[PO_THRU])
                        ba.xvel *= -1;
                }

                //Was hit on bottom
                if (dist[2] < dist[0] && dist[2] < dist[1] && dist[2] < dist[3]) {
                    ba.posy = br.posy - br.height - ba.height;
                    if (ba.yvel > 0 && !player.powerup[PO_THRU])
                        ba.yvel *= -1;
                }

                //Was hit on top
                if (dist[3] < dist[0] && dist[3] < dist[1] && dist[3] < dist[2]) {
                    ba.posy = br.posy + br.height + ba.height;
                    if (ba.yvel < 0 && !player.powerup[PO_THRU])
                        ba.yvel *= -1;
                }

                //Setup vars for spawning powerups
                pos a, b;
                a.x = br.posx;
                a.y = br.posy;

                //Hastigheden en powerup blier sendt afsted med

                if (player.difficulty == EASY) {
                    b.x = ba.xvel / 2.0;
                    b.y = ba.yvel / 2.0;
                } else if (player.difficulty == NORMAL) {
                    b.x = ba.xvel / 1.5;
                    b.y = ba.yvel / 1.5;
                } else //if(player.difficulty == HARD)
                {
                    b.x = ba.xvel / 1.25;
                    b.y = ba.yvel / 1.25;
                }

                if (dirfound) {
                    if (ba.explosive) {
                        br.type = 'B';
                    }

                    //Update p, used by caller to find out if we hit anything..
                    p.x = ba.posx + px;
                    p.y = ba.posy + py;


                    ba.hit(br.tex.prop.glParColorInfo);

                    if (!player.powerup[PO_THRU] || player.difficulty == HARD) {
                        ba.setspeed(ba.velocity + difficulty.hitbrickinc[player.difficulty]);
                    }
                } else {
                    SDL_Log("Collision detection error: Don't know where the ball hit.");
                }
                br.hit(fxMan, a, b, 1);
            } //collision
        } //x boxcol
    } //y boxcol
}

void padcoldet(ball &b, paddle_class &p, pos &po) {
    int i, points = 0;
    GLfloat x, y, px = 0, py = 0;
    bool col = 0;
    //Er bolden tæt nok på?

    if (b.posy < (p.posy + p.height) + b.height && b.posy > p.posy - p.height) {
        if (b.posx > p.posx - (p.width * 2.0) - b.width && b.posx < p.posx + (p.width * 2.0) + b.width) {
            for (i = 0; i < 32; i++) {
                x = b.bsin[i];
                y = b.bcos[i];

                //Find de punkter der er inden i padden
                if (b.posx + x > p.posx - p.width && b.posx + x < p.posx + p.width) {
                    if (b.posy + y < p.posy + p.height && b.posy + y > p.posy - p.height) {
                        col = 1;

                        px += x;
                        py += y;
                        points++;
                    }
                }
            } //For loop

            if (col) {
                col = 0;
                gVar.deadTime = 0;
                px /= (float) points;
                py /= (float) points;

                px = b.posx + px;

                //Ved at reagere herinde fungerer yvel som en switch, så det kun sker een gang ;)
                if (b.yvel < 0) {
                    b.posy = p.posy + p.height + b.height; //løft op over pad

                    //Only decrease speed if the player does not have the go-thru powerup
                    if (!player.powerup[PO_THRU]) {
                        b.setspeed(b.velocity + difficulty.hitpaddleinc[player.difficulty]);
                    }

                    b.setangle(bounceOffAngle(p.width, p.posx, b.posx));
                    if (player.powerup[PO_GLUE]) {
                        b.gluedX = p.posx + p.width - px;
                        b.glued = 1;
                    }

                    po.x = px;
                    po.y = py;
                }
            }
        }
    }
}

#include "highscores.cpp"

struct shopItemStruct {
    int price;
    int type;
};

class hudClass {
    textureClass texBall;

    // For the hud text
    int ticksSinceLastClockCheck;
    time_t nixTime; //Seconds since epoch
    tm timeStruct; //Time struct
    char clockString[13]; //Clock: 00:00\0


    //For the powerup "shop"
    textureClass *texPowerup; //Pointer to array of powerup textures
    int shopItemSelected;
#define NUMITEMSFORSALE 13
    struct shopItemStruct item[NUMITEMSFORSALE];
    bool shopItemBlocked[NUMITEMSFORSALE]; //One can only buy each powerup one time each life/level

public:
    hudClass(textureClass texB, textureClass texPo[]) {
        texPowerup = texPo;
        texBall = texB;
        ticksSinceLastClockCheck = 1001;

        item[0].type = PO_LASER;
        item[0].price = 600;
        item[1].type = PO_NORMALBALL;
        item[1].price = 750;
        item[2].type = PO_BIGBALL;
        item[2].price = 800;
        item[3].type = PO_AIMHELP;
        item[3].price = 900;
        item[4].type = PO_GROWPADDLE;
        item[4].price = 960;
        item[5].type = PO_MULTIBALL;
        item[5].price = 980;
        item[6].type = PO_EXPLOSIVE_GROW;
        item[6].price = 990;
        item[7].type = PO_EXPLOSIVE;
        item[7].price = 1000;
        item[8].type = PO_GLUE;
        item[8].price = 1000;
        item[9].type = PO_EASYBRICK;
        item[9].price = 2000;
        item[10].type = PO_GUN;
        item[10].price = 3000;
        item[11].type = PO_THRU;
        item[11].price = 4000;
        item[12].type = PO_LIFE;
        item[12].price = 6000;

        shopItemSelected = 0;
    }

    void draw() {
        int i;
        //Draw lives left.
        glLoadIdentity();
        glTranslatef(0, 0, 0.0);
        glColor4f(texBall.prop.glTexColorInfo[0], texBall.prop.glTexColorInfo[1], texBall.prop.glTexColorInfo[2],
                  texBall.prop.glTexColorInfo[3]);

        glBindTexture(GL_TEXTURE_2D, texBall.prop.texture);
        texBall.play();
        glBegin(GL_QUADS);
        for (i = 0; i < player.lives - 1; i++) {
            glTexCoord2f(texBall.pos[0], texBall.pos[1]);
            glVertex3f(1.55 - (0.05 * i), 1.2, 0.0);
            glTexCoord2f(texBall.pos[2], texBall.pos[3]);
            glVertex3f(1.5 - (0.05 * i), 1.2, 0.0);
            glTexCoord2f(texBall.pos[4], texBall.pos[5]);
            glVertex3f(1.5 - (0.05 * i), 1.15, 0.0);
            glTexCoord2f(texBall.pos[6], texBall.pos[7]);
            glVertex3f(1.55 - (0.05 * i), 1.15, 0.0);
        }
        glEnd();

        if (setting.showClock) {
            ticksSinceLastClockCheck += globalTicksSinceLastDraw;
            if (ticksSinceLastClockCheck > 1000) {
                ticksSinceLastClockCheck = 0;
                time(&nixTime);
                timeStruct = *(localtime(&nixTime));
                sprintf(clockString, "%02i:%02i", timeStruct.tm_hour, timeStruct.tm_min);
            }
            glText->write(clockString, FONT_INTRODESCRIPTION, false, 1.0f, -0.95f,
                          -0.975f);
        }

        //Draw the "shop"
        //First, find out how many items the player can afford, so we can center them
        int canAfford = 0;
        for (i = 0; i < NUMITEMSFORSALE; i++) {
            if (item[i].price <= player.coins) {
                canAfford++;
            }
        }

        if (shopItemSelected > canAfford || shopItemSelected < 0) {
            shopItemSelected = canAfford - 1;
        }

        GLfloat shopListStartX = -((0.11 * canAfford) / 2.0);
        if (gVar.shopNextItem) {
            gVar.shopNextItem = 0;
            shopItemSelected++;

            if (shopItemSelected > canAfford - 1) {
                shopItemSelected = 0;
            }
        } else if (gVar.shopPrevItem) {
            gVar.shopPrevItem = 0;
            shopItemSelected--;

            if (shopItemSelected < 0) {
                shopItemSelected = canAfford - 1;
                if (shopItemSelected < 0) {
                    shopItemSelected = 0;
                }
            }
        } else if (gVar.shopBuyItem) {
            gVar.shopBuyItem = 0;
            if (item[shopItemSelected].price <= player.coins && !shopItemBlocked[shopItemSelected]) {
                pos a, b;
                a.x = shopListStartX + (0.11 * shopItemSelected);
                a.y = 1.15;
                b.x = 0.0;
                b.y = 0.0;
                pMan.spawn(a, b, item[shopItemSelected].type);
                player.coins -= item[shopItemSelected].price;
                shopItemBlocked[shopItemSelected] = true;
                gVar.shopNextItem = true;
                soundMan.add(SND_BUY_POWERUP, 0.0);
            }
        }

        glTranslatef(shopListStartX, 1.0f, 0.0f);
        for (i = 0; i < canAfford; i++) {
            if (i == shopItemSelected) {
                if (shopItemBlocked[i]) {
                    glColor4f(GL_FULL_RED);
                } else {
                    glColor4f(1.0, 1.0, 1.0, 1.0);
                }
            } else {
                if (shopItemBlocked[i]) {
                    glColor4f(1.0, 0.0, 0.0, 0.4);
                } else {
                    glColor4f(1.0, 1.0, 1.0, 0.4);
                }
            }
            texPowerup[item[i].type].play();
            glBindTexture(GL_TEXTURE_2D, texPowerup[item[i].type].prop.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(texPowerup[item[i].type].pos[0], texPowerup[item[i].type].pos[1]);
            glVertex3f(-0.055, 0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].pos[2], texPowerup[item[i].type].pos[3]);
            glVertex3f(0.055, 0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].pos[4], texPowerup[item[i].type].pos[5]);
            glVertex3f(0.055, -0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].pos[6], texPowerup[item[i].type].pos[7]);
            glVertex3f(-0.055, -0.055, 0.00);
            glEnd();
            glTranslatef(0.11, 0.0, 0.0f);
        }
    }

    void clearShop() {
        for (bool &i: shopItemBlocked) {
            i = false;
        }
    }
};

class speedometerClass {
public:
    static void draw() {
        //GLfloat y = -1.24 + difficulty.maxballspeed[player.difficulty]/2.44*var.averageBallSpeed;

        const GLfloat y = 0.48 / (difficulty.maxballspeed[player.difficulty] - difficulty.ballspeed[player.difficulty])
                          * (
                              var.averageBallSpeed - difficulty.ballspeed[player.difficulty]);

        glDisable(GL_TEXTURE_2D);

        glLoadIdentity();
        glTranslatef(1.0f, -1.0, 0.0);
        glBegin(GL_QUADS);
        glColor4f(0, 1, 0, 1);
        glVertex3f(0, y, 0);
        glVertex3f(0.03, y, 0);
        glColor4f(GL_WHITE);
        glVertex3f(0.03, 0, 0);
        glVertex3f(0, 0, 0);
        glEnd();

        glEnable(GL_TEXTURE_2D);
    }
};

void detonateExplosives(brick bricks[], effectManager &fxMan) {
    struct pos p, v;
    for (int i = 0; i < 598; i++) {
        if (bricks[i].active && bricks[i].type == 'B') {
            p.x = bricks[i].posx;
            p.y = bricks[i].posy;
            v.x = 0.0;
            v.y = 0.0;
            bricks[i].hit(fxMan, p, v, 0);
        }
    }
}

void updateBrickPositions(brick bricks[]) {
    for (int i = 0; i < 598; i++) {
        if (!bricks[i].active) continue;

        // Original Fallgeschwindigkeit beibehalten
        bricks[i].posy -= bricks[i].height * 2;

        // Original untere Grenze beibehalten
        if (bricks[i].posy < -1.00 - bricks[i].height) {
            bricks[i].active = false;
            updated_nbrick[bricks[i].row][bricks[i].bricknum] = -1;
            player.score -= bricks[i].score;

            var.bricksHit = true;
            gVar.deadTime = 0;
        }
    }
}

void explosiveGrow(brick bricks[]) {
    for (int i = 0; i < 598; i++) {
        if (bricks[i].active) {
            bricks[i].growExplosive(bricks);
        }
    }

    for (int i = 0; i < 598; i++) {
        bricks[i].justBecomeExplosive = 0;
    }
}

void easyBrick(brick bricks[]) {
    for (int i = 0; i < 598; i++) {
        if (bricks[i].active) {
            bricks[i].breakable();
        }
    }
}

#include "input.cpp"
#include "title.cpp"

bool screenShot() {
    static constexpr size_t MAX_FILENAME = 256;
    static constexpr size_t TGA_HEADER_SIZE = 12;
    static constexpr size_t TGA_INFO_SIZE = 6;
    static constexpr size_t CHANNELS = 3; // BGR

    char fileName[MAX_FILENAME];
    int fileIndex = 0;

    // Finde freien Dateinamen
    while (fileIndex < 9999) {
        const int result = snprintf(fileName, MAX_FILENAME, "%s/sdl-ball_%04d.tga",
                                    configFile.getScreenshotDir().data(), fileIndex);

        if (result < 0 || static_cast<size_t>(result) >= MAX_FILENAME) {
            SDL_Log("Filename too long");
            return false;
        }

        FILE *test = fopen(fileName, "rb");
        if (!test) break;
        fclose(test);
        fileIndex++;
    }

    if (fileIndex == 9999) {
        SDL_Log("No free filename found");
        return false;
    }

    // Alloziere Pixel Buffer
    const size_t pixelCount = setting.res_x * setting.res_y * CHANNELS;
    const auto pixels = std::make_unique<GLubyte[]>(pixelCount);
    if (!pixels) {
        SDL_Log("Memory allocation failed");
        return false;
    }

    // Öffne Ausgabedatei
    FILE *outFile = fopen(fileName, "wb");
    if (!outFile) {
        SDL_Log("Could not create file '%s'", fileName);
        return false;
    }

    // Lese Framebuffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, setting.res_x, setting.res_y, GL_BGR, GL_UNSIGNED_BYTE, pixels.get());

    if (glGetError() != GL_NO_ERROR) {
        SDL_Log("Failed reading OpenGL framebuffer");
        fclose(outFile);
        return false;
    }

    // Schreibe TGA Header
    const unsigned char tgaHeader[TGA_HEADER_SIZE] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const unsigned char tgaInfo[TGA_INFO_SIZE] = {
        static_cast<unsigned char>(setting.res_x & 0xFF),
        static_cast<unsigned char>((setting.res_x >> 8) & 0xFF),
        static_cast<unsigned char>(setting.res_y & 0xFF),
        static_cast<unsigned char>((setting.res_y >> 8) & 0xFF),
        24, 0
    };

    if (fwrite(tgaHeader, 1, TGA_HEADER_SIZE, outFile) != TGA_HEADER_SIZE ||
        fwrite(tgaInfo, 1, TGA_INFO_SIZE, outFile) != TGA_INFO_SIZE ||
        fwrite(pixels.get(), 1, pixelCount, outFile) != pixelCount) {
        SDL_Log("Failed writing TGA file");
        fclose(outFile);
        return false;
    }

    fclose(outFile);
    return true;
}

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    if (!configFile.init()) {
        SDL_Log("Fehler beim Initialisieren der Konfigurationspfade");
    }

    if (!settingsManager.init()) {
        SDL_Log("Fehler beim Laden der Einstellungen - verwende Standardwerte");
    }

    setting = settingsManager.getSettings();
    player.difficulty = setting.startingDifficulty;
    var.quit = false;
    var.clearScreen = true;
    var.titleScreenShow = true;

    Uint32 maxFrameAge = 1000 / setting.fps;

    GLfloat mouse_x, mouse_y;

    static_difficulty.ballspeed[EASY] = 0.7f;
    static_difficulty.ballspeed[NORMAL] = 1.3f;
    static_difficulty.ballspeed[HARD] = 1.6f;

    static_difficulty.maxballspeed[EASY] = 1.5f;
    static_difficulty.maxballspeed[NORMAL] = 2.2f;
    static_difficulty.maxballspeed[HARD] = 3.0f;

    static_difficulty.hitbrickinc[EASY] = 0.0025;
    static_difficulty.hitbrickinc[NORMAL] = 0.003;
    static_difficulty.hitbrickinc[HARD] = 0.004;

    static_difficulty.hitpaddleinc[EASY] = -0.001;
    static_difficulty.hitpaddleinc[NORMAL] = -0.0005;
    static_difficulty.hitpaddleinc[HARD] = -0.0007;

    //Percentage
    static_difficulty.speedup[EASY] = 10.0f;
    static_difficulty.speedup[NORMAL] = 20.0f;
    static_difficulty.speedup[HARD] = 30.0f;

    difficulty = static_difficulty;

    soundMan.init();

    if (!display.init()) {
        var.quit = true;
    }
    initGL();
    SDL_SetWindowIcon(display.sdlWindow, IMG_Load(themeManager.getThemeFilePath("icon32.png", setting.gfxTheme).data()));
    SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);

#ifndef DEBUG_NO_RELATIVE_MOUSE
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

    glText = new glTextClass; // instantiate the class now that settings have been read.

    textureManager texMgr;

    textureClass texPaddleBase;
    textureClass texPaddleLayers[2];
    textureClass texBall[3];
    textureClass texLvl[13];
    texExplosiveBrick = &texLvl[0];

    textureClass texBorder;
    textureClass texPowerup[MAXPOTEXTURES];
    textureClass texBullet;
    textureClass texParticle;

    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/paddle/base.txt", setting.gfxTheme), texPaddleBase);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/paddle/glue.txt", setting.gfxTheme), texPaddleLayers[0]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/paddle/gun.txt", setting.gfxTheme), texPaddleLayers[1]);

    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/ball/normal.txt", setting.gfxTheme), texBall[0]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/ball/fireball.txt", setting.gfxTheme), texBall[1]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/ball/tail.txt", setting.gfxTheme), texBall[2]);

    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/explosive.txt", setting.gfxTheme), texLvl[0]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/base.txt", setting.gfxTheme), texLvl[1]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/cement.txt", setting.gfxTheme), texLvl[2]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/doom.txt", setting.gfxTheme), texLvl[3]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/glass.txt", setting.gfxTheme), texLvl[4]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/invisible.txt", setting.gfxTheme), texLvl[5]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/blue.txt", setting.gfxTheme), texLvl[6]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/yellow.txt", setting.gfxTheme), texLvl[7]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/green.txt", setting.gfxTheme), texLvl[8]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/grey.txt", setting.gfxTheme), texLvl[9]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/purple.txt", setting.gfxTheme), texLvl[10]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/white.txt", setting.gfxTheme), texLvl[11]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/brick/red.txt", setting.gfxTheme), texLvl[12]);

    texMgr.load(themeManager.getThemeFilePath("gfx/border.png", setting.gfxTheme), texBorder);

    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/coin.txt", setting.gfxTheme), texPowerup[PO_COIN]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/glue.txt", setting.gfxTheme), texPowerup[PO_GLUE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/multiball.txt", setting.gfxTheme), texPowerup[PO_MULTIBALL]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/bigball.txt", setting.gfxTheme), texPowerup[PO_BIGBALL]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/normalball.txt", setting.gfxTheme), texPowerup[PO_NORMALBALL]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/smallball.txt", setting.gfxTheme), texPowerup[PO_SMALLBALL]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/aim.txt", setting.gfxTheme), texPowerup[PO_AIM]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/explosive.txt", setting.gfxTheme), texPowerup[PO_EXPLOSIVE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/gun.txt", setting.gfxTheme), texPowerup[PO_GUN]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/go-thru.txt", setting.gfxTheme), texPowerup[PO_THRU]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/laser.txt", setting.gfxTheme), texPowerup[PO_LASER]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/life.txt", setting.gfxTheme), texPowerup[PO_LIFE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/die.txt", setting.gfxTheme), texPowerup[PO_DIE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/drop.txt", setting.gfxTheme), texPowerup[PO_DROP]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/detonate.txt", setting.gfxTheme), texPowerup[PO_DETONATE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/explosive-grow.txt", setting.gfxTheme), texPowerup[PO_EXPLOSIVE_GROW]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/easybrick.txt", setting.gfxTheme), texPowerup[PO_EASYBRICK]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/nextlevel.txt", setting.gfxTheme), texPowerup[PO_NEXTLEVEL]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/aimhelp.txt", setting.gfxTheme), texPowerup[PO_AIMHELP]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/growbat.txt", setting.gfxTheme), texPowerup[PO_GROWPADDLE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/shrinkbat.txt", setting.gfxTheme), texPowerup[PO_SHRINKPADDLE]);
    texMgr.readTexProps(themeManager.getThemeFilePath("gfx/powerup/bullet.txt", setting.gfxTheme), texBullet);
    pMan.init(texPowerup);

    texMgr.load(themeManager.getThemeFilePath("gfx/particle.png", setting.gfxTheme), texParticle);

    GLuint sceneDL;
    createPlayfieldBorder(&sceneDL, texBorder);

    brick bricks[598];

    string levelfile = themeManager.getThemeFilePath("levels.txt", setting.lvlTheme);

    int i = 0; //bruges i for loop xD
    glScoreBoard scoreboard;
    menuClass menu;
    paddle_class paddle;

    paddle.tex = texPaddleBase;
    paddle.layerTex = texPaddleLayers;

    effectManager fxMan;
    fxMan.set(FX_VAR_TEXTURE, texParticle);
    fxMan.set(FX_VAR_GRAVITY, 0.6f);

    titleScreenClass titleScreen(&fxMan, texPowerup, &menu);

    ballManager bMan(texBall);

    initNewGame();
    paddle.posy = -1.15;

    // This is GOING to be containing the "hud" (score, borders, lives left, level, speedometer)
    highScoreClass hKeeper;
    backgroundClass bg;
    bulletsClass bullet(texBullet);
    speedometerClass speedo;
    hudClass hud(texBall[0], texPowerup);

    var.effectnum = -1;

    Uint32 lastTick = SDL_GetTicks();
    Uint32 nonpausingLastTick = lastTick;
    char txt[256];
    Uint32 frameAge = 0; // in milliseconds

#ifdef performanceTimer
  struct timeval timeStart,timeStop;
  int renderTime;
#endif

    controllerClass control(&paddle, &bullet, &bMan);

    menu.joystickAttached = control.joystickAttached();

    soundMan.add(SND_START, 0);

    // Todo show in title
    SDL_Log("SDL-Ball v %s", VERSION);
    SDL_Event event;
    while (!var.quit) {
#ifdef performanceTimer
    gettimeofday(&timeStart, NULL);
#endif
        // Events
        control.get(); //Check for keypresses and joystick events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    var.quit = true;
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        display.resize(
                            event.window.data1,
                            event.window.data2
                        );
                    }
                    break;
                default: ;
            }
            if (event.type == SDL_KEYDOWN) {
                if (var.showHighScores) {
                    hKeeper.type(event, menu);
                } else if (var.enterSaveGameName) {
                    menu.enterSaveGameName(event);
                } else {
                    if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_PAUSE) {
                        var.paused ? resumeGame() : pauseGame();
                    }

                    if (event.key.keysym.sym == SDLK_q) {
                        var.quit = true;
                    } else if (event.key.keysym.sym == SDLK_s) {
                        screenShot();
                    } else if (event.key.keysym.sym == setting.keyNextPo) {
                        gVar.shopPrevItem = true;
                    } else if (event.key.keysym.sym == setting.keyBuyPo) {
                        gVar.shopBuyItem = true;
                    } else if (event.key.keysym.sym == setting.keyPrevPo) {
                        gVar.shopNextItem = true;
                    } else if (event.key.keysym.sym == SDLK_u) {
                        var.clearScreen ? var.clearScreen = false : var.clearScreen = true;
                    } else if (event.key.keysym.sym == SDLK_c) {
                        setting.showClock ? setting.showClock = false : setting.showClock = true;
                        if (!settingsManager.save()) {
                            SDL_Log("Error saving settings");
                        }
                    }
                }

                //Toggle menu
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (var.titleScreenShow)
                        var.titleScreenShow = false;
                    switch (var.menu) {
                        case 0:
                            var.menu = 1;
                            pauseGame();
                            break;
                        case 1:
                            resumeGame();
                            break;
                        default:
                            var.menu = 1;
                            break;
                    }
                } else if (event.key.keysym.sym == SDLK_F1) {
                    if (!var.titleScreenShow) {
                        var.titleScreenShow = true;
                        pauseGame();
                    } else {
                        var.titleScreenShow = false;
                        resumeGame();
                    }
                } else if (event.key.keysym.sym == SDLK_F11) {
                    if (setting.fullscreen) {
                        setting.fullscreen = false;
                        SDL_SetWindowFullscreen(display.sdlWindow, 0);
                    } else {
                        setting.fullscreen = true;
                        SDL_SetWindowFullscreen(display.sdlWindow, SDL_WINDOW_FULLSCREEN);
                    }
                }
            }

            if (event.type == SDL_MOUSEMOTION) {
                mouse_x = (event.motion.x - display.currentW / 2) * display.glunits_per_xpixel;
                mouse_y = (event.motion.y - display.currentH / 2) * display.glunits_per_ypixel * -1;
#ifdef DEBUG_SHOW_MOUSE_COORDINATES
                SDL_Log("Mouse:%s%s,%s%s", setw(10), mousex, setw(10), mousey);
                SDL_Log("%s%s,%s%s", setw(8), event.motion.x, setw(8), event.motion.y);
#endif
                if (var.menu) {
                    if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.78) + (0.07) && mouse_y > (-0.78) - (0.07))
                        var.menuItem = 1;
                    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.56) + (0.07) && mouse_y > (-0.56) - (
                                 0.07))
                        var.menuItem = 2;
                    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.34) + (0.07) && mouse_y > (-0.34) - (
                                 0.07))
                        var.menuItem = 3;
                    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.12) + (0.07) && mouse_y > (-0.12) - (
                                 0.07))
                        var.menuItem = 4;
                    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (0.1) + (0.07) && mouse_y > (0.1) - (0.07))
                        var.menuItem = 5;
                    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (0.32) + (0.07) && mouse_y > (0.32) - (0.07))
                        var.menuItem = 6;
                    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (0.54) + (0.07) && mouse_y > (0.54) - (0.07))
                        var.menuItem = 7;
                    else
                        var.menuItem = 0;
                } else {
                    control.movePaddle(paddle.posx + event.motion.xrel * display.glunits_per_xpixel);
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (var.menu) {
                        var.menuPressed = true;
                        if (var.menuItem > 0)
                            soundMan.add(SND_MENUCLICK, 0);
                    }
                    control.btnPress();
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    gVar.shopBuyItem = true;
                } else if (event.button.button == 4) {
                    gVar.shopPrevItem = true;
                } else if (event.button.button == 5) {
                    gVar.shopNextItem = true;
                }
            }
        }

        // Timing
        nonpausingGlobalTicks = SDL_GetTicks() - nonpausingLastTick;
        frameAge += nonpausingGlobalTicks;

        nonpausingGlobalMilliTicks = nonpausingGlobalTicks / 1000.0f;
        nonpausingLastTick = SDL_GetTicks();

        if (!var.paused) {
            globalTicks = SDL_GetTicks() - lastTick;
            globalMilliTicks = globalTicks / 1000.0;
        } else {
            globalTicks = globalMilliTicks = 0;
        }
        lastTick = SDL_GetTicks();

        globalTicksSinceLastDraw += nonpausingGlobalTicks;
        globalMilliTicksSinceLastDraw += nonpausingGlobalMilliTicks;

        // Update
        gVar.deadTime += globalTicks;

        glLoadIdentity();
        // Really ugly... but easy
        if (!var.titleScreenShow) {
            pos p;

            if (gVar.deadTime > 20000) {
                //give the balls explosive ability, in order to blow up cement block and get on with the game
                gVar.deadTime = 0;
                bMan.powerup(PO_EXPLOSIVE);
            }

            if (bMan.activeBalls == 0 && !gVar.newLevel)
            // only check if we are dead if we have actually started playing
            {
                player.lives--;
                if (player.lives > 0) {
                    resetPlayerPowerups();
                    gVar.newLife = true;
                    if (!paddle.dead)
                        player.explodePaddle = true;
                    pMan.die(fxMan);
                } else if (!gVar.gameOver) {
                    gVar.gameOver = true;
                    pauseGame();
                    if (hKeeper.isHighScore()) {
                        // announce.write(string text, int mslife, int fontnum);
                        announce.write("Highscore!", 3000,FONT_ANNOUNCE_GOOD);
                        var.showHighScores = 1;
                        soundMan.add(SND_HIGHSCORE, 0);
                    }
                } else if (gVar.gameOver && !var.showHighScores) {
                    //Only ran if there is gameover and no highscore
                    if (var.effectnum == -1) {
                        fxMan.set(FX_VAR_TYPE, FX_TRANSIT);
                        fxMan.set(FX_VAR_LIFE, 3000);
                        fxMan.set(FX_VAR_COLOR, 0.0, 0.0, 0.0);
                        p.x = 0.0;
                        p.y = 0.0;

                        //Kør en transition effekt
                        var.effectnum = fxMan.spawn(p);
                        announce.write("GameOver!", 1500,FONT_ANNOUNCE_BAD);
                        soundMan.add(SND_GAMEOVER, 0);
                    } else {
                        if (var.transition_half_done) {
                            var.titleScreenShow = true;
                            fxMan.kill(var.effectnum);
                            var.effectnum = -1;
                            initNewGame();
                            resumeGame();
                        }
                    }
                }
            }

            if (gVar.nextlevel) {
                if (var.effectnum == -1) {
                    announce.write("Well Done!", 1000, FONT_ANNOUNCE_GOOD);
                    soundMan.add(SND_NEXTLEVEL, 0);

                    if (bMan.activeBalls > 1) {
                        sprintf(txt, "Bonus: %i", bMan.activeBalls * 150);
                        player.score += (bMan.activeBalls * 150) * player.multiply;
                        announce.write(txt, 2000, FONT_ANNOUNCE_GOOD);
                    }

                    fxMan.set(FX_VAR_TYPE, FX_TRANSIT);
                    fxMan.set(FX_VAR_LIFE, 1600);
                    fxMan.set(FX_VAR_COLOR, 0.0, 0.0, 0.0);
                    p.x = 0.0;
                    p.y = 0.0;

                    //Kør en transition effekt
                    var.effectnum = fxMan.spawn(p);

                    var.idiotlock = false;
                } else {
                    if (var.transition_half_done) {
                        if (!var.idiotlock) {
                            var.idiotlock = true;
                            player.level++;
                            SOLPlayer = player; // Capture how the player is at the start of this level
                            // If the player completed all levels, restart the game with higher multiplier
                            if (player.level == var.numlevels) {
                                player.multiply += player.multiply * 3;
                                player.level = 0;
                                announce.write("Finished!", 3500,FONT_ANNOUNCE_GOOD);
                            }

                            sprintf(txt, "Level %i", player.level + 1); // +1 fordi levels starter fra 0
                            announce.write(txt, 1500,FONT_ANNOUNCE_GOOD);

                            //check om vi skal fjerne powerups
                            if (player.difficulty > EASY) {
                                resetPlayerPowerups();
                            }
                            gVar.newLevel = true;
                            var.paused = false;
                        }
                    }

                    if (!fxMan.isActive(var.effectnum)) {
                        var.effectnum = -1; //nulstil så den er klar igen
                        gVar.nextlevel = false;
                        var.paused = false;
                        var.idiotlock = false;
                    }
                }
            }

            if (gVar.newLevel) {
                var.bricksHit = true;
                gVar.newLevel = false;
                loadlevel(levelfile, bricks, player.level);
                initlevels(bricks, texLvl);
                gVar.gameOver = false;
                gVar.newLife = true;
                pMan.clear();
                bullet.clear();
                paddle.posx = 0.0;
                var.startedPlaying = false;
                bg.init(texMgr);
                hud.clearShop();
            }

            if (gVar.newLife) {
                gVar.newLife = false;
                paddle.init();
                p.x = paddle.posx;

                p.y = paddle.posy + paddle.height + 0.025f;

                bMan.clear();
                bMan.spawn(p, true, paddle.width, difficulty.ballspeed[player.difficulty], 1.57100000f);
                //Not exactly 90 degree, so the ball will always fall a bit to the side
            }

            if (frameAge >= maxFrameAge) {
                if (var.clearScreen) {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }

                // Update player score
                scoreboard.update(player.score);

                // Background
                if (setting.showBg)
                    bg.draw();

                // Borders
                glCallList(sceneDL);

                if (var.scrollInfo.drop) {
                    if (SDL_GetTicks() - var.scrollInfo.lastTick > var.scrollInfo.dropspeed) {
                        var.scrollInfo.lastTick = SDL_GetTicks();
                        updateBrickPositions(bricks);
                    }
                }

                if (gVar.bricksleft == 1) {
                    player.powerup[PO_AIMHELP] = true;
                }
            }

            bullet.move();

            gVar.bricksleft = 0;

            // Update nbrick here
            if (var.bricksHit) {
                memcpy(nbrick, updated_nbrick, sizeof(updated_nbrick));
                var.bricksHit = false;
            }

            for (i = 0; i < 598; i++) {
                if (bricks[i].active) {
                    if (bricks[i].destroytowin && bricks[i].hitsLeft) {
                        gVar.bricksleft++;
                    }

                    if (bricks[i].collide) {
                        bMan.bcoldet(bricks[i], fxMan);
                        //bullets
                        if (player.powerup[PO_GUN]) {
                            bullet.coldet(bricks[i], fxMan);
                        }

                        //check kollision på effekterne
                        if (setting.particleCollide && setting.eyeCandy && frameAge >= maxFrameAge)
                            fxMan.coldet(bricks[i]);
                    }
                    if (frameAge >= maxFrameAge)
                        bricks[i].draw(bricks, fxMan);
                } //aktiv brik
            } // for loop

            // Collision between paddle and balls
            if (bMan.pcoldet(paddle, fxMan)) {
                if (player.powerup[PO_DROP]) {
                    updateBrickPositions(bricks);
                }
            }

            bMan.move();

            if (setting.particleCollide && setting.eyeCandy && frameAge >= maxFrameAge)
                fxMan.pcoldet(paddle);

            pMan.move();
            if (pMan.coldet(paddle, fxMan, bMan)) {
                if (player.powerup[PO_DETONATE]) {
                    player.powerup[PO_DETONATE] = false;
                    detonateExplosives(bricks, fxMan);
                }

                if (player.powerup[PO_EASYBRICK]) {
                    player.powerup[PO_EASYBRICK] = false;
                    easyBrick(bricks);
                }

                if (player.powerup[PO_NEXTLEVEL]) {
                    player.powerup[PO_NEXTLEVEL] = false;
                    gVar.nextlevel = true;
                    var.paused = true;
                }

                if (player.powerup[PO_EXPLOSIVE_GROW]) {
                    player.powerup[PO_EXPLOSIVE_GROW] = false;
                    explosiveGrow(bricks);
                }
            }

            if (frameAge >= maxFrameAge) {
                soundMan.play();

                if (player.explodePaddle) {
                    player.explodePaddle = false;
                    soundMan.add(SND_DIE, 0);
                    if (setting.eyeCandy) {
                        fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);

                        p.x = paddle.width * 2;
                        p.y = paddle.height * 2;
                        fxMan.set(FX_VAR_RECTANGLE, p);

                        p.x = paddle.posx;
                        p.y = paddle.posy;

                        fxMan.set(FX_VAR_LIFE, 2000);
                        fxMan.set(FX_VAR_NUM, 20);
                        fxMan.set(FX_VAR_SIZE, 0.025f);
                        fxMan.set(FX_VAR_SPEED, 0.35f);
                        fxMan.set(FX_VAR_GRAVITY, -0.7f);
                        fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
                        fxMan.spawn(p);
                        fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
                        fxMan.spawn(p);
                        fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
                        fxMan.spawn(p);
                        fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
                        fxMan.spawn(p);

                        fxMan.set(FX_VAR_NUM, 32);
                        fxMan.set(FX_VAR_SIZE, 0.05f);
                        fxMan.set(FX_VAR_LIFE, 1500);
                        fxMan.set(FX_VAR_SPEED, 0.7f);
                        fxMan.set(FX_VAR_GRAVITY, 0.0f);

                        fxMan.set(FX_VAR_COLOR, 0.5f, 0.5f, 0.5f);
                        fxMan.spawn(p);
                        fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f);
                        fxMan.spawn(p);
                    }
                }

                bMan.updatelast();
                glColor3d(255, 255, 255);
                bullet.draw();
                paddle.draw();

                pMan.draw();
                bMan.draw(paddle);
                scoreboard.draw();
                speedo.draw();
                hud.draw();
                fxMan.draw();

                if (var.showHighScores)
                    hKeeper.draw();

                if (var.menu > 0) {
                    if (var.menu == 10 || var.menu == 11) {
                        control.calibrate();
                    }
                    menu.doMenu();
                }

                announce.draw();
                SDL_GL_SwapWindow(display.sdlWindow);
                frameAge = 0;
                globalTicksSinceLastDraw = 0;
                globalMilliTicksSinceLastDraw = 0;
#ifdef performanceTimer
          SDL_Log("FrameAge:");
      } else {
        SDL_Log("LoopAge:");
      }
      gettimeofday(&timeStop, NULL);
      renderTime = timeStop.tv_usec - timeStart.tv_usec;
      SDL_Log("%s", renderTime);
#else
            }
#endif
            if (!gVar.bricksleft) {
                gVar.nextlevel = true;
                var.paused = true;
            }
        } else {
            //Show the title screen
            titleScreen.draw(&frameAge, &maxFrameAge);
        }
        usleep(1000);
    }

    // todo GUI ask, instead of want to quit, if changes
    if (settingsManager.hasChanged()) {
        if (!settingsManager.save()) {
            SDL_Log("Fehler beim Speichern der Einstellungen");
        }
    }
    delete glText;
    return EXIT_SUCCESS;
}
