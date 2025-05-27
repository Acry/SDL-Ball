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
#include <random>
#include <sys/stat.h>
#include <unistd.h>

#include "BackgroundManager.h"
#include "Ball.h"
#include "ConfigFileManager.h"
#include "Display.hpp"
#include "GameObject.h"
#include "MathHelper.h"
#include "MovingObject.h"
#include "Paddle.h"
#include "SaveGameManager.h"
#include "Score.h"
#include "SettingsManager.h"
#include "SoundManager.h"
#include "Speedometer.h"
#include "SpriteSheetAnimation.h"
#include "TextureManager.h"
#include "ThemeManager.h"
#include "TextManager.h"
#include "colors.h"
#include "config.h"
#include "TextureProperties.h"

#define DEBUG_DRAW_BALL_QUAD 0
#define DEBUG_SHOW_MOUSE_COORDINATES 0 // NDC coordinates -1.0 to 1.0

using namespace std;

// Option 1: Struct mit benannten Koordinaten
// struct TexCoords {
//     GLfloat s1, t1;  // Texturkoordinaten Vertex 1
//     GLfloat s2, t2;  // Texturkoordinaten Vertex 2
//     GLfloat s3, t3;  // Texturkoordinaten Vertex 3
//     GLfloat s4, t4;  // Texturkoordinaten Vertex 4
// };
typedef GLfloat texPos[8];

class EffectManager;

void makeExplosive(brick & b);

// nasty fix to a problem
int nbrick[23][26];
int updated_nbrick[23][26];

class brick : public GameObject {
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
    bool justBecomeExplosive; //If this brick just becomes an explosive one.

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

    void hit(EffectManager &fxMan, position poSpawnPos, position poSpawnVel, bool ballHitMe);

    void draw(brick bricks[], EffectManager &fxMan) {
        if (isdyingnormally) {
            fade -= fadespeed * globalMilliTicksSinceLastDraw;
            opacity = fade;
            zoom -= zoomspeed * globalMilliTicksSinceLastDraw;

            if (fade < 0.0)
                active = false;
        }

        if (isexploding && !var.paused) {
            fade -= 7.0f * globalMilliTicksSinceLastDraw;
            opacity = fade;
            if (fade < 0.0f) {
                active = false;

                position spos, svel;
                spos.x = pos_x;
                spos.y = pos_y;
                if (bricknum > 0) {
                    if (nbrick[row][bricknum - 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row][bricknum - 1]].hit(fxMan, spos, svel, false);
                    }
                }
                if (bricknum < 25) {
                    if (nbrick[row][bricknum + 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row][bricknum + 1]].hit(fxMan, spos, svel, false);
                    }
                }
                if (row > 0) {
                    if (nbrick[row - 1][bricknum] != -1) {
                        svel.x = random_float(2, 0) / 3.0;
                        svel.y = random_float(2, 0) / 3.0;
                        bricks[nbrick[row - 1][bricknum]].hit(fxMan, spos, svel, false);
                    }
                }
                if (row < 22) {
                    if (nbrick[row + 1][bricknum] != -1) {
                        svel.x = random_float(2.0f, 0.0f) / 3.0f;
                        svel.y = random_float(2.0f, 0.0f) / 3.0f;
                        bricks[nbrick[row + 1][bricknum]].hit(fxMan, spos, svel, false);
                    }
                }
                if (row > 0 && bricknum > 0) {
                    if (nbrick[row - 1][bricknum - 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0f;
                        svel.y = random_float(2, 0) / 3.0f;
                        bricks[nbrick[row - 1][bricknum - 1]].hit(fxMan, spos, svel, false);
                    }
                }
                if (row > 0 && bricknum < 25) {
                    if (nbrick[row - 1][bricknum + 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0f;
                        svel.y = random_float(2, 0) / 3.0f;
                        bricks[nbrick[row - 1][bricknum + 1]].hit(fxMan, spos, svel, false);
                    }
                }
                if (row < 22 && bricknum > 0) {
                    if (nbrick[row + 1][bricknum - 1] != -1) {
                        svel.x = random_float(2, 0) / 3.0f;
                        svel.y = random_float(2, 0) / 3.0f;
                        bricks[nbrick[row + 1][bricknum - 1]].hit(fxMan, spos, svel, false);
                    }
                }
                if (row < 22 && bricknum < 25) {
                    if (nbrick[row + 1][bricknum + 1] != -1) {
                        svel.x = random_float(2.0f, 0.0f) / 3.0f;
                        svel.y = random_float(2.0f, 0.0f) / 3.0f;
                        bricks[nbrick[row + 1][bricknum + 1]].hit(fxMan, spos, svel, false);
                    }
                }
            }
        }
        texture.play();
        glColor4f(texture.textureProperties.glTexColorInfo[0], texture.textureProperties.glTexColorInfo[1],
                  texture.textureProperties.glTexColorInfo[2], opacity);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
        glLoadIdentity();
        glBegin(GL_QUADS);
        // Berechne die Ecken des Quads
        // glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection, flipping bottom and top for SDL2
        //         +1
        //         ^
        //         |
        // -1 <----+----> +1
        //         |
        //        -1
        const float left = pos_x * zoom;
        const float right = pos_x + BRICK_WIDTH * zoom;
        const float top = pos_y * zoom;
        const float bottom = pos_y + BRICK_HEIGHT * zoom;

        // Zeichne das Quad mit den Texturkoordinaten
        glTexCoord2f(texture.texturePosition[0], texture.texturePosition[1]);
        glVertex3f(left, top, 0.0f); // Oben links

        glTexCoord2f(texture.texturePosition[2], texture.texturePosition[3]);
        glVertex3f(right, top, 0.0f); // Oben rechts

        glTexCoord2f(texture.texturePosition[4], texture.texturePosition[5]);
        glVertex3f(right, bottom, 0.0f); // Unten rechts

        glTexCoord2f(texture.texturePosition[6], texture.texturePosition[7]);
        glVertex3f(left, bottom, 0.0f); // Unten links
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    void growExplosive(brick bricks[]) const {
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
            texture.frame = 2;
            texture.play();
        } else if (type == '4') {
            hitsLeft = 1;
            texture.frame = 2;
            texture.play();
        } else if (type == '9') {
            hitsLeft = 1;
            texture.frame = 3;
            texture.play();
        }
    }
};

void collision_ball_brick(brick &br, Ball &ba, position &p, EffectManager &fxMan);

// Powerups
class PowerupClass : public MovingObject {
public:
    int score;
    int type;
    int level, maxlevel;
    GLfloat gravity;

    PowerupClass() {
        pos_x = 0.0;
        pos_y = 0.0;
        xvel = 0.0;
        yvel = 0.0;
        width = 0.055f / 1.25f;
        height = 0.055f / 1.25f;
    }

    void move() {
        //grav
        yvel -= gravity * globalMilliTicks;
        //SDL_Log("%s", yvel);
        pos_x += xvel * globalMilliTicks;
        pos_y += yvel * globalMilliTicks;
    }


    bool coldet(Paddle &p, EffectManager &fxMan, BallManager &bMan) {
        bool col = false;
        if (pos_x + width > 1.0f && xvel > 0.0) {
            col = true;
            xvel *= -1;
        } else if (pos_x - width < -1.0f && xvel < 0.0) {
            col = true;
            xvel *= -1;
        } else if (pos_y + width > 1.0f && yvel > 0.0) {
            col = true;
            yvel *= -1;
        } else if (pos_y - width < -1.0f) {
            active = false;
        }

        if (col) {
            soundManager.add(SND_PO_HIT_BORDER, pos_x);
        }

        //idiotisk lavet...

        bool ycol = false;
        bool xcol = false;

        //En side
        if (pos_x + width > p.pos_x - p.width && pos_x + width < p.pos_x + p.width) {
            xcol = true;
        }

        if (pos_x - width > p.pos_x - p.width && pos_x - width < p.pos_x + p.width) {
            xcol = true;
        }

        if (pos_y - height < p.pos_y + p.height && pos_y - height > p.pos_y - p.height) {
            ycol = true;
        }

        if (pos_y + height < p.pos_y + p.height && pos_y + height > p.pos_y - p.height) {
            ycol = true;
        }

        if (xcol && ycol) {
            if (setting.eyeCandy) {
                position fxSize;
                position fxpos;
                fxpos.x = pos_x;
                fxpos.y = pos_y;
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

                fxMan.set(FX_VAR_COLOR, texture.textureProperties.glParColorInfo[0],
                          texture.textureProperties.glParColorInfo[1],
                          texture.textureProperties.glParColorInfo[2]);
                fxMan.spawn(fxpos);
            }
            active = false;

            //Score
            player.score += score * player.multiply;
            //Apply powerup:
            switch (type) {
                case PO_COIN:
                    player.coins += 1000;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_GLUE:
                    player.coins += 150;
                    player.powerup[PO_GLUE] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_BIGBALL:
                    player.coins += 30;
                    bMan.applyPowerup(PO_BIGBALL);
                    player.powerup[PO_BIGBALL] = true;
                    player.powerup[PO_NORMALBALL] = false;
                    player.powerup[PO_SMALLBALL] = false;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_NORMALBALL:
                    player.coins += 50;
                    bMan.applyPowerup(PO_NORMALBALL);
                    player.powerup[PO_NORMALBALL] = true;
                    player.powerup[PO_BIGBALL] = false;
                    player.powerup[PO_SMALLBALL] = false;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_SMALLBALL:
                    player.coins += 10;
                    bMan.applyPowerup(PO_SMALLBALL);
                    player.powerup[PO_SMALLBALL] = true;
                    player.powerup[PO_BIGBALL] = false;
                    player.powerup[PO_NORMALBALL] = false;
                    soundManager.add(SND_EVIL_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_MULTIBALL:
                    player.coins += 100;
                    bMan.multiply();
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_AIM:
                    player.coins += 50;
                    if (player.difficulty == 0) {
                        player.powerup[PO_GLUE] = true;
                    }
                    if (!player.powerup[PO_AIM]) {
                        player.powerup[PO_AIM] = true;
                        player.powerup[PO_LASER] = true;
                    } else {
                        player.powerup[PO_GLUE] = true;
                    }
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_GROWPADDLE:
                    player.coins += 100;
                    if (p.width < 0.4) p.grow(p.width + 0.03);
                    player.powerup[PO_GUN] = false;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_SHRINKPADDLE:
                    player.coins += 10;
                    if (p.width > 0.02) p.grow(p.width - 0.02);
                    player.powerup[PO_GUN] = false;
                    soundManager.add(SND_EVIL_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_EXPLOSIVE:
                    player.coins += 150;
                    bMan.applyPowerup(PO_EXPLOSIVE);
                    player.powerup[PO_EXPLOSIVE] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_GUN:
                    player.coins += 200;
                    player.powerup[PO_GUN] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_THRU:
                    player.coins += 300;
                    player.powerup[PO_THRU] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_LASER:
                    player.coins += 40;
                    player.powerup[PO_LASER] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_LIFE:
                    player.coins += 400;
                    player.lives++;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_DIE:
                    player.coins += 1;
                    player.explodePaddle = true;
                    player.powerup[PO_DIE] = true;
                    //NOTE: no sound here, SND_DIE is played when paddle dissapers
                    break;
                case PO_DROP:
                    player.coins += 1;
                    player.powerup[PO_DROP] = true;
                    soundManager.add(SND_EVIL_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_DETONATE:
                    player.coins += 200;
                    player.powerup[PO_DETONATE] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_EXPLOSIVE_GROW:
                    player.coins += 100;
                    player.powerup[PO_EXPLOSIVE_GROW] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_EASYBRICK:
                    player.coins += 90;
                    player.powerup[PO_EASYBRICK] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                case PO_NEXTLEVEL:
                    player.coins += 100;
                    player.powerup[PO_NEXTLEVEL] = true;
                    //NOTE: no sound here, SND_NEXTLEVEL is played when changing level
                    break;
                case PO_AIMHELP:
                    player.coins += 50;
                    player.powerup[PO_AIMHELP] = true;
                    soundManager.add(SND_GOOD_PO_HIT_PADDLE, pos_x);
                    break;
                default: ;
            }
            return true;
        }

        return false;
    }

    void die(EffectManager &fxMan) {
        active = false;
        if (setting.eyeCandy) {
            position p;
            p.x = pos_x;
            p.y = pos_y;
            fxMan.set(FX_VAR_TYPE, FX_SPARKS);
            fxMan.set(FX_VAR_COLDET, 1);
            fxMan.set(FX_VAR_LIFE, 1250);
            fxMan.set(FX_VAR_NUM, 16);

            fxMan.set(FX_VAR_SPEED, 0.8f);
            fxMan.set(FX_VAR_GRAVITY, 0.6f);
            fxMan.set(FX_VAR_SIZE, 0.025f);
            fxMan.set(FX_VAR_COLOR, texture.textureProperties.glParColorInfo[0],
                      texture.textureProperties.glParColorInfo[1], texture.textureProperties.glParColorInfo[2]);
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
        texture.play();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
        glColor4f(texture.textureProperties.glTexColorInfo[0], texture.textureProperties.glTexColorInfo[1],
                  texture.textureProperties.glTexColorInfo[2],
                  texture.textureProperties.glTexColorInfo[3]);
        glLoadIdentity();
        glTranslatef(pos_x, pos_y, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(texture.texturePosition[0], texture.texturePosition[1]);
        glVertex3f(-width, height, 0.00); // øverst venst
        glTexCoord2f(texture.texturePosition[2], texture.texturePosition[3]);
        glVertex3f(width, height, 0.00); // øverst højre
        glTexCoord2f(texture.texturePosition[4], texture.texturePosition[5]);
        glVertex3f(width, -height, 0.00); // nederst højre
        glTexCoord2f(texture.texturePosition[6], texture.texturePosition[7]);
        glVertex3f(-width, -height, 0.00); // nederst venstre
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
};

class PowerupManager {
    int i;
    PowerupClass p[MAXPOWERUPS];
    SpriteSheetAnimation *tex;

public:
    void init(SpriteSheetAnimation texPowerup[]) {
        tex = new SpriteSheetAnimation[40];
        tex = texPowerup;
    }

    PowerupManager() {
        clear();
    }

    void clear() {
        for (i = 0; i < MAXPOWERUPS; i++) {
            p[i].active = false;
        }
    }

    void die(EffectManager &fxMan) {
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (p[i].active) {
                p[i].die(fxMan);
            }
        }
    }

    void spawn(position spawnpos, position velocity, int type) {
        for (i = 0; i < MAXPOWERUPS; i++) {
            if (!p[i].active) {
                p[i].gravity = 0.7;
                p[i].type = type;
                p[i].pos_x = spawnpos.x;
                p[i].pos_y = spawnpos.y;
                p[i].xvel = velocity.x * -1;
                p[i].yvel = velocity.y * -1;
                p[i].active = true;

                //Give texture that this type has.
                p[i].texture = tex[type];

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

    int coldet(Paddle &paddle, EffectManager &fxMan, BallManager &bMan) {
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

    void spawnPowerup(const char powerup, const position a, const position b) {
        switch (powerup) {
            case '1': spawn(a, b, PO_GROWPADDLE);
                break;
            case '2': spawn(a, b, PO_SHRINKPADDLE);
                break;
            case '3': spawn(a, b, PO_DIE);
                break;
            case '4': spawn(a, b, PO_GLUE);
                break;
            case '5': spawn(a, b, PO_MULTIBALL);
                break;
            case '6': spawn(a, b, PO_THRU);
                break;
            case '7': spawn(a, b, PO_DROP);
                break;
            case '8': spawn(a, b, PO_DETONATE);
                break;
            case '9': spawn(a, b, PO_EXPLOSIVE_GROW);
                break;
            case 'A': spawn(a, b, PO_EASYBRICK);
                break;
            case 'B': spawn(a, b, PO_EXPLOSIVE);
                break;
            case 'C': spawn(a, b, PO_NEXTLEVEL);
                break;
            case 'D': spawn(a, b, PO_AIMHELP);
                break;
            case 'E': spawn(a, b, PO_COIN);
                break;
            case 'F': spawn(a, b, PO_BIGBALL);
                break;
            case 'G': spawn(a, b, PO_NORMALBALL);
                break;
            case 'H': spawn(a, b, PO_SMALLBALL);
                break;
            case 'I': spawn(a, b, PO_AIM);
                break;
            case 'O': spawn(a, b, PO_LIFE);
                break;
            case 'P': spawn(a, b, PO_GUN);
                break;
            case 'R': spawn(a, b, PO_LASER);
                break;
            default: break;
        }
    }
};

PowerupManager powerupManager;

SpriteSheetAnimation *texExplosiveBrick; //NOTE:Ugly

class Bullet {
    MovingObject bullets[16];

public:
    int active;

    Bullet(const SpriteSheetAnimation &texBullet) {
        for (auto &bullet: bullets) {
            bullet.active = false;
            bullet.texture = texBullet;
            bullet.width = 0.02;
            bullet.height = 0.02;
        }
    }

    void shoot(const position p) {
        //Find ledig bullet
        for (auto &bullet: bullets) {
            if (!bullet.active) {
                soundManager.add(SND_SHOT, p.x);
                bullet.active = true;
                bullet.pos_x = p.x;
                bullet.pos_y = p.y;
                bullet.xvel = 0;
                bullet.yvel = 1.0;
                break;
            }
        }
    }

    void move() {
        for (auto &bullet: bullets) {
            if (bullet.active) {
                //Flyt
                bullet.pos_y += bullet.yvel * globalMilliTicks;
            }
        }
    }

    void draw() {
        glColor4f(GL_WHITE);
        for (auto &bullet: bullets) {
            if (bullet.active) {
                //draw

                bullet.texture.play();

                glLoadIdentity();
                glTranslatef(bullet.pos_x, bullet.pos_y, 0.0);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, bullet.texture.textureProperties.texture);
                glBegin(GL_QUADS);
                glTexCoord2f(bullet.texture.texturePosition[0], bullet.texture.texturePosition[1]);
                glVertex3f(-bullet.width, bullet.height, 0.0);
                glTexCoord2f(bullet.texture.texturePosition[2], bullet.texture.texturePosition[3]);
                glVertex3f(bullet.width, bullet.height, 0.0);
                glTexCoord2f(bullet.texture.texturePosition[4], bullet.texture.texturePosition[5]);
                glVertex3f(bullet.width, -bullet.height, 0.0);
                glTexCoord2f(bullet.texture.texturePosition[6], bullet.texture.texturePosition[7]);
                glVertex3f(-bullet.width, -bullet.height, 0.0);
                glDisable(GL_TEXTURE_2D);
                glEnd();
            }
        }
    }

    void clear() {
        for (auto &bullet: bullets) {
            bullet.active = false;
        }
    }

    void coldet(brick &b, EffectManager &fxMan) {
        position v;
        v.x = 0;
        v.y = bullets[0].xvel;

        for (auto &bullet: bullets) {
            if (bullet.active) {
                //y
                if (bullet.pos_y + bullet.height / 10.0 > b.pos_y - b.height && bullet.pos_y + bullet.
                    height / 10.0 < b.pos_y + b.height) {
                    bool hit = false;
                    position p;
                    p.x = b.pos_x;
                    p.y = b.pos_y;
                    //Venstre side:
                    if (bullet.pos_x > b.pos_x - b.width && bullet.pos_x < b.pos_x + b.width) {
                        hit = true;
                    }
                    if (hit) {
                        b.hit(fxMan, p, v, true);
                        bullet.active = false;
                        p.x = bullet.pos_x;
                        p.y = bullet.pos_y;
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
                } else if (bullet.pos_y > 1.0f) {
                    bullet.active = false;
                }
            }
        }
    }
};

void brick::hit(EffectManager &fxMan, position poSpawnPos, position poSpawnVel, bool ballHitMe) {
    position p, s;

    if (type != '3' || player.powerup[PO_THRU])
        hitsLeft--;

    // We don't want to play a sound if this brick is not an explosive, and was hit by an explosion
    if (ballHitMe || type == 'B') {
        if (type == '3') //cement
        {
            soundManager.add(SND_CEMENT_BRICK_HIT, pos_x);
        } else if (type == '4' || type == '9') //glass or invisible
        {
            if (hitsLeft == 2) {
                soundManager.add(SND_INVISIBLE_BRICK_APPEAR, pos_x);
            } else if (hitsLeft == 1) {
                soundManager.add(SND_GLASS_BRICK_HIT, pos_x);
            } else {
                soundManager.add(SND_GLASS_BRICK_BREAK, pos_x);
            }
        } else if (type == 'B') //explosive
        {
            soundManager.add(SND_EXPL_BRICK_BREAK, pos_x);
        } else if (type == 'C') //Doom brick
        {
            soundManager.add(SND_DOOM_BRICK_BREAK, pos_x);
        } else {
            //All the other bricks
            soundManager.add(SND_NORM_BRICK_BREAK, pos_x);
        }
    }


    if (type != '3' || player.powerup[PO_THRU]) {
        //Brick was hit, dont do anything
        if (isdyingnormally || isexploding) {
            return;
        }
        player.score += score * player.multiply * var.averageBallSpeed; //Speed bonus

        if (hitsLeft < 1 || type == 'B') //Hvis brikken er explosiv kan den ikke have nogle hits tilbage
        {
            collide = false;

            updated_nbrick[row][bricknum] = -1;
            var.bricksHit = true;

            gVar.deadTime = 0;

            powerupManager.spawnPowerup(powerup, poSpawnPos, poSpawnVel);
            powerup = '0';

            if (setting.eyeCandy) {
                p.x = pos_x;
                p.y = pos_y;
                s.x = width * 1.2f; // Von 2.0 auf 1.2 reduziert
                s.y = height * 1.2f; // Von 2.0 auf 1.2 reduziert

                fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
                fxMan.set(FX_VAR_COLDET, 1);
                fxMan.set(FX_VAR_LIFE, 800); // Von 1300 auf 800 reduziert
                fxMan.set(FX_VAR_NUM, 12); // Von 20 auf 12 reduziert
                fxMan.set(FX_VAR_SIZE, 0.015f); // Von 0.03 auf 0.015 reduziert
                fxMan.set(FX_VAR_SPEED, 0.4f); // Von 0.6 auf 0.4 reduziert
                fxMan.set(FX_VAR_GRAVITY, 0.5f); // Von 0.7 auf 0.5 reduziert

                fxMan.set(FX_VAR_RECTANGLE, s);
                fxMan.set(FX_VAR_COLOR, texture.textureProperties.glParColorInfo[0],
                          texture.textureProperties.glParColorInfo[1],
                          texture.textureProperties.glParColorInfo[2]);
                fxMan.spawn(p);
            }

            if (type == 'B') {
                isexploding = true;

                if (setting.eyeCandy) {
                    p.x = pos_x;
                    p.y = pos_y;
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
                isdyingnormally = true;
            }
        } else {
            //No hits left
            texture.frame++;
            texture.play();
        } //Hits left
    }
}

void collision_ball_brick(brick &br, Ball &ba, position &p, EffectManager &fxMan) {
    // Early exit wenn Brick nicht aktiv
    if (!br.active || !br.collide) return;

    // AABB Collision Check (Axis Aligned Bounding Box)
    bool collision = (ba.pos_x - ba.width < br.pos_x + br.width &&
                      ba.pos_x + ba.width > br.pos_x - br.width &&
                      ba.pos_y - ba.height < br.pos_y + br.height &&
                      ba.pos_y + ba.height > br.pos_y - br.height);

    if (!collision) return;

    // Kollisionsrichtung bestimmen
    float overlapX = min(ba.pos_x + ba.width - (br.pos_x - br.width),
                         (br.pos_x + br.width) - (ba.pos_x - ba.width));
    float overlapY = min(ba.pos_y + ba.height - (br.pos_y - br.height),
                         (br.pos_y + br.height) - (ba.pos_y - ba.height));

    // Position für Powerup-Spawn
    position spawnPos = {br.pos_x, br.pos_y};
    position spawnVel = {ba.xvel * 0.5f, ba.yvel * 0.5f};

    // Reflektion basierend auf Kollisionsrichtung
    if (overlapX < overlapY) {
        ba.xvel *= -1; // Horizontale Reflektion
    } else {
        ba.yvel *= -1; // Vertikale Reflektion
    }

    // Brick Hit
    br.hit(fxMan, spawnPos, spawnVel, true);

    // Ball Hit Effect
    ba.hit(br.texture.textureProperties.glParColorInfo);
}

struct shopItemStruct {
    int price;
    int type;
};

class Hud {
    SpriteSheetAnimation texBall;
    TextManager &text; // Referenz auf das Singleton
    // For the hud text
    int ticksSinceLastClockCheck;
    time_t nixTime; //Seconds since epoch
    tm timeStruct; //Time struct
    char clockString[13]; //Clock: 00:00\0
    SpriteSheetAnimation *texPowerup; //Pointer to array of powerup textures
    int shopItemSelected;
    shopItemStruct item[NUMITEMSFORSALE];
    bool shopItemBlocked[NUMITEMSFORSALE]; //One can only buy each powerup one time each life/level

public:
    Hud(SpriteSheetAnimation texB, SpriteSheetAnimation texPo[]): texBall(texB),
                                                                  text(TextManager::getInstance()),
                                                                  ticksSinceLastClockCheck(1001),
                                                                  texPowerup(texPo),
                                                                  shopItemSelected(0) {
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
    }

    void draw() {
        int i;
        glLoadIdentity();
        //Draw lives left.
        glColor4f(texBall.textureProperties.glTexColorInfo[0], texBall.textureProperties.glTexColorInfo[1],
                  texBall.textureProperties.glTexColorInfo[2],
                  texBall.textureProperties.glTexColorInfo[3]);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texBall.textureProperties.texture);
        texBall.play();
        glBegin(GL_QUADS);
        for (i = 0; i < player.lives - 1; i++) {
            constexpr float size = 0.05f; // Größere Bälle für bessere Sichtbarkeit
            constexpr float space = 0.05f; // Mehr Abstand zwischen den Bällen
            constexpr float y = 1.0f - space; // Etwas tiefer von der oberen Kante

            // Bälle werden von rechts nach links gezeichnet
            glTexCoord2f(texBall.texturePosition[0], texBall.texturePosition[1]);
            glVertex3f(0.9f - space * i, y, 0.0); // Oben rechts
            glTexCoord2f(texBall.texturePosition[2], texBall.texturePosition[3]);
            glVertex3f(0.9f - size - space * i, y, 0.0); // Oben links
            glTexCoord2f(texBall.texturePosition[4], texBall.texturePosition[5]);
            glVertex3f(0.9f - size - space * i, y - size, 0.0); // Unten links
            glTexCoord2f(texBall.texturePosition[6], texBall.texturePosition[7]);
            glVertex3f(0.9f - space * i, y - size, 0.0); // Unten rechts
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
        if (setting.showClock) {
            ticksSinceLastClockCheck += globalTicksSinceLastDraw;
            if (ticksSinceLastClockCheck > 1000) {
                ticksSinceLastClockCheck = 0;
                time(&nixTime);
                timeStruct = *(localtime(&nixTime));
                sprintf(clockString, "%02i:%02i", timeStruct.tm_hour, timeStruct.tm_min);
            }
            text.write(clockString, FONT_INTRODESCRIPTION, false, 1.5f, -0.92f,
                       -0.97f);
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

        const GLfloat shopListStartX = -(0.11f * canAfford / 1.0f);
        if (gVar.shopNextItem) {
            gVar.shopNextItem = false;
            shopItemSelected++;

            if (shopItemSelected > canAfford - 1) {
                shopItemSelected = 0;
            }
        } else if (gVar.shopPrevItem) {
            gVar.shopPrevItem = false;
            shopItemSelected--;

            if (shopItemSelected < 0) {
                shopItemSelected = canAfford - 1;
                if (shopItemSelected < 0) {
                    shopItemSelected = 0;
                }
            }
        } else if (gVar.shopBuyItem) {
            gVar.shopBuyItem = false;
            if (item[shopItemSelected].price <= player.coins && !shopItemBlocked[shopItemSelected]) {
                position a, b;
                a.x = shopListStartX + (0.11f * shopItemSelected);
                a.y = 1.15f;
                b.x = 0.0f;
                b.y = 0.0f;
                powerupManager.spawn(a, b, item[shopItemSelected].type);
                player.coins -= item[shopItemSelected].price;
                shopItemBlocked[shopItemSelected] = true;
                gVar.shopNextItem = true;
                soundManager.add(SND_BUY_POWERUP, 0.0f);
            }
        }
        glLoadIdentity();
        glTranslatef(shopListStartX, 0.9f, 0.0f);
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
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texPowerup[item[i].type].textureProperties.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[0], texPowerup[item[i].type].texturePosition[1]);
            glVertex3f(-0.055, 0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[2], texPowerup[item[i].type].texturePosition[3]);
            glVertex3f(0.055, 0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[4], texPowerup[item[i].type].texturePosition[5]);
            glVertex3f(0.055, -0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[6], texPowerup[item[i].type].texturePosition[7]);
            glVertex3f(-0.055, -0.055, 0.00);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glTranslatef(0.11, 0.0, 0.0f);
        }
    }

    void clearShop() {
        for (bool &i: shopItemBlocked) {
            i = false;
        }
    }
};

void detonateExplosives(brick bricks[], EffectManager &fxMan) {
    for (int i = 0; i < 598; i++) {
        if (bricks[i].active && bricks[i].type == 'B') {
            position v;
            position p;
            p.x = bricks[i].pos_x;
            p.y = bricks[i].pos_y;
            v.x = 0.0;
            v.y = 0.0;
            bricks[i].hit(fxMan, p, v, false);
        }
    }
}

void updateBrickPositions(brick bricks[]) {
    for (int i = 0; i < 598; i++) {
        if (!bricks[i].active) continue;

        // Original Fallgeschwindigkeit beibehalten
        bricks[i].pos_y -= bricks[i].height * 2;

        // Original untere Grenze beibehalten
        if (bricks[i].pos_y < -1.00 - bricks[i].height) {
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
        bricks[i].justBecomeExplosive = false;
    }
}

void easyBrick(brick bricks[]) {
    for (int i = 0; i < 598; i++) {
        if (bricks[i].active) {
            bricks[i].breakable();
        }
    }
}

// Braucht
// Paddle *paddle;
// bulletsClass *bullet;
// BallManager *bMan;
#include "Controller.cpp"
// braucht effectmanager
#include "TitleScreen.cpp"

#include "HighScore.cpp"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    if (!configFileManager.init()) {
        SDL_Log("Fehler beim Initialisieren der Konfigurationspfade");
    }

    if (!settingsManager.init()) {
        SDL_Log("Fehler beim Laden der Einstellungen - verwende Standardwerte");
    }
    Player player(configFileManager);
    setting = settingsManager.getSettings();

    var.quit = false;
    var.clearScreen = true;
    var.titleScreenShow = true;

    // CAREFUL: When I initialize this after display.init(), rendering was broken for me.
    soundManager.init();

    if (!display.init()) {
        var.quit = true;
    }
    SDL_SetWindowIcon(display.sdlWindow,
                      IMG_Load(themeManager.getThemeFilePath("icon32.png", setting.gfxTheme).data()));
    SDL_WarpMouseInWindow(display.sdlWindow, display.currentW / 2, display.currentH / 2);

#ifndef DEBUG_NO_RELATIVE_MOUSE
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

#if DEBUG_SHOW_MOUSE_COORDINATES
#include "DebugMouse.cpp"
    DebugMouse debugMouse;
#endif
#pragma region texture manager
    SpriteSheetAnimation texPaddleBase;
    SpriteSheetAnimation texPaddleLayers[2];
    SpriteSheetAnimation texBall[3];
    SpriteSheetAnimation texLvl[13];
    texExplosiveBrick = &texLvl[0];

    SpriteSheetAnimation texBorder;
    SpriteSheetAnimation texPowerup[MAXPOTEXTURES];
    SpriteSheetAnimation texBullet;
    SpriteSheetAnimation texParticle;
    TextureManager textureManager;
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/paddle/base.txt", setting.gfxTheme), texPaddleBase);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/paddle/glue.txt", setting.gfxTheme),
                                texPaddleLayers[0]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/paddle/gun.txt", setting.gfxTheme),
                                texPaddleLayers[1]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/ball/normal.txt", setting.gfxTheme), texBall[0]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/ball/fireball.txt", setting.gfxTheme), texBall[1]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/ball/tail.txt", setting.gfxTheme), texBall[2]);

    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/explosive.txt", setting.gfxTheme), texLvl[0]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/base.txt", setting.gfxTheme), texLvl[1]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/cement.txt", setting.gfxTheme), texLvl[2]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/doom.txt", setting.gfxTheme), texLvl[3]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/glass.txt", setting.gfxTheme), texLvl[4]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/invisible.txt", setting.gfxTheme), texLvl[5]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/blue.txt", setting.gfxTheme), texLvl[6]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/yellow.txt", setting.gfxTheme), texLvl[7]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/green.txt", setting.gfxTheme), texLvl[8]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/grey.txt", setting.gfxTheme), texLvl[9]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/purple.txt", setting.gfxTheme), texLvl[10]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/white.txt", setting.gfxTheme), texLvl[11]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/brick/red.txt", setting.gfxTheme), texLvl[12]);

    textureManager.load(themeManager.getThemeFilePath("gfx/border.png", setting.gfxTheme), texBorder);

    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/coin.txt", setting.gfxTheme),
                                texPowerup[PO_COIN]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/glue.txt", setting.gfxTheme),
                                texPowerup[PO_GLUE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/multiball.txt", setting.gfxTheme),
                                texPowerup[PO_MULTIBALL]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/bigball.txt", setting.gfxTheme),
                                texPowerup[PO_BIGBALL]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/normalball.txt", setting.gfxTheme),
                                texPowerup[PO_NORMALBALL]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/smallball.txt", setting.gfxTheme),
                                texPowerup[PO_SMALLBALL]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/aim.txt", setting.gfxTheme),
                                texPowerup[PO_AIM]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/explosive.txt", setting.gfxTheme),
                                texPowerup[PO_EXPLOSIVE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/gun.txt", setting.gfxTheme),
                                texPowerup[PO_GUN]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/go-thru.txt", setting.gfxTheme),
                                texPowerup[PO_THRU]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/laser.txt", setting.gfxTheme),
                                texPowerup[PO_LASER]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/life.txt", setting.gfxTheme),
                                texPowerup[PO_LIFE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/die.txt", setting.gfxTheme),
                                texPowerup[PO_DIE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/drop.txt", setting.gfxTheme),
                                texPowerup[PO_DROP]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/detonate.txt", setting.gfxTheme),
                                texPowerup[PO_DETONATE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/explosive-grow.txt", setting.gfxTheme),
                                texPowerup[PO_EXPLOSIVE_GROW]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/easybrick.txt", setting.gfxTheme),
                                texPowerup[PO_EASYBRICK]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/nextlevel.txt", setting.gfxTheme),
                                texPowerup[PO_NEXTLEVEL]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/aimhelp.txt", setting.gfxTheme),
                                texPowerup[PO_AIMHELP]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/growbat.txt", setting.gfxTheme),
                                texPowerup[PO_GROWPADDLE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/shrinkbat.txt", setting.gfxTheme),
                                texPowerup[PO_SHRINKPADDLE]);
    textureManager.readTextureProperties(themeManager.getThemeFilePath("gfx/powerup/bullet.txt", setting.gfxTheme), texBullet);
    textureManager.load(themeManager.getThemeFilePath("gfx/particle.png", setting.gfxTheme), texParticle);
# pragma endregion texture manager

    powerupManager.init(texPowerup);


    brick bricks[598];
    string levelFile = themeManager.getThemeFilePath("levels.txt", setting.lvlTheme);
    // todo: make property of levelManager ()

    // part of levelManager wip in loadlevel_new.cpp
    // if (!load_levels()) {;
    //     SDL_Log("Error loading levels-structure");
    //     var.quit = true;
    // }

    int i = 0; // bruges i for loop xD
    Score score;
    Menu menu;
    Paddle paddle;
    paddle.texture = texPaddleBase;
    paddle.layerTex = texPaddleLayers;

    effectMananger.set(FX_VAR_TEXTURE, texParticle);
    effectMananger.set(FX_VAR_GRAVITY, 0.6f);

    TitleScreen titleScreen(&effectMananger, texPowerup, &menu);
    BallManager ballManager(texBall);
    BackgroundManager background;
    Bullet bullet(texBullet);

    // "hud" (score, borders, lives left, level, speedometer)
    HighScore hKeeper;
    Speedometer speedo;
    Hud hud(texBall[0], texPowerup);

    var.effectnum = -1;
    GLfloat normalizedMouseX, normalizedMouseY;
    Uint32 maxFrameAge = 1000 / setting.fps;
    Uint32 lastTick = SDL_GetTicks();
    Uint32 nonpausingLastTick = lastTick;
    char txt[256];
    Uint32 frameAge = 0; // in milliseconds

    Controller controller(&paddle, &bullet, &ballManager);
    menu.joystickAttached = controller.joystickAttached();
    soundManager.add(SND_START, 0);

    // Todo show in title
    SDL_Log("SDL-Ball v %s", VERSION);
    SDL_Event event;
    initNewGame();
    while (!var.quit) {
#pragma region events
        controller.get(); //Check for keypresses and joystick events
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
                        display.screenshot();
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
                    }
                }
                //Toggle menu
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (var.titleScreenShow)
                        var.titleScreenShow = false;
                    switch (var.menuShown) {
                        case 0:
                            var.menuShown = 1;
                            pauseGame();
                            break;
                        case 1:
                            resumeGame();
                            break;
                        default:
                            var.menuShown = 1;
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
                normalizedMouseX = (event.motion.x - display.currentW / 2.0f);
                normalizedMouseY = (event.motion.y - display.currentH / 2.0f) * -1;
                normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * (
                                       2.0f / display.viewportW);
                normalizedMouseY = (event.motion.y - display.viewportY - display.viewportH / 2.0f) * -1 * (
                                       2.0f / display.viewportH);
                normalizedMouseX = std::max(-1.0f, std::min(1.0f, normalizedMouseX));
                normalizedMouseY = std::max(-1.0f, std::min(1.0f, normalizedMouseY));

                if (var.menuShown) {
                    // we got no objects with position here
                    if (normalizedMouseX > -MENU_HALF_WIDTH && normalizedMouseX < MENU_HALF_WIDTH) {
                        for (int m = 1; m <= 7; m++) {
                            constexpr float START_Y = -0.62f;
                            if (float centerY = START_Y + MENU_SPACING * (m - 1);
                                normalizedMouseY < centerY + MENU_HALF_HEIGHT &&
                                normalizedMouseY > centerY - MENU_HALF_HEIGHT) {
                                var.menuItemHovered = m;
                                break;
                            }
                        }
                    } else var.menuItemHovered = 0;
                } else {
                    controller.movePaddle(paddle.pos_x + event.motion.xrel * display.glunits_per_xpixel);
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (var.menuShown) {
                        var.menuPressed = true;
                        if (var.menuItemHovered > 0)
                            soundManager.add(SND_MENUCLICK, 0);
                    }
                    controller.btnPress();
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    gVar.shopBuyItem = true;
                } else if (event.button.button == 4) {
                    gVar.shopPrevItem = true;
                } else if (event.button.button == 5) {
                    gVar.shopNextItem = true;
                }
            }
        }
#pragma endregion events

#pragma region timing
        // Timing
        nonpausingGlobalTicks = SDL_GetTicks() - nonpausingLastTick;
        frameAge += nonpausingGlobalTicks;

        nonpausingGlobalMilliTicks = nonpausingGlobalTicks / 1000.0f;
        nonpausingLastTick = SDL_GetTicks();
        if (!var.paused) {
            globalTicks = SDL_GetTicks() - lastTick;
            globalMilliTicks = globalTicks / 1000.0f;
        } else {
            globalTicks = globalMilliTicks = 0.0f;
        }
        lastTick = SDL_GetTicks();
        globalTicksSinceLastDraw += nonpausingGlobalTicks;
        globalMilliTicksSinceLastDraw += nonpausingGlobalMilliTicks;

        // Update
        gVar.deadTime += globalTicks;
#pragma endregion

#pragma region update & rendering
        // Hideous... but simple
        if (!var.titleScreenShow) {
            position p;

            if (gVar.deadTime > 20000) {
                // give the balls explosive ability, to blow up cement block and get on with the game
                gVar.deadTime = 0;
                ballManager.applyPowerup(PO_EXPLOSIVE);
            }

            if (ballManager.activeBalls == 0 && !gVar.newLevel)
            // only check if we are dead if we have actually started playing
            {
                player.lives--;
                if (player.lives > 0) {
                    resetPlayerPowerups();
                    gVar.newLife = true;
                    if (!paddle.dead)
                        player.explodePaddle = true;
                    powerupManager.die(effectMananger);
                } else if (!gVar.gameOver) {
                    gVar.gameOver = true;
                    pauseGame();
                    if (HighScore::isHighScore()) {
                        // announce.write(string text, int mslife, int fontnum);
                        announce.write("Highscore!", 3000, FONT_ANNOUNCE_GOOD);
                        var.showHighScores = 1;
                        soundManager.add(SND_HIGHSCORE, 0);
                    }
                } else if (gVar.gameOver && !var.showHighScores) {
                    // Only ran if there is gameover and no highscore
                    if (var.effectnum == -1) {
                        effectMananger.set(FX_VAR_TYPE, FX_TRANSIT);
                        effectMananger.set(FX_VAR_LIFE, 3000);
                        effectMananger.set(FX_VAR_COLOR, 0.0, 0.0, 0.0);
                        p.x = 0.0;
                        p.y = 0.0;

                        // Kør en transition effekt
                        var.effectnum = effectMananger.spawn(p);
                        announce.write("GameOver!", 1500, FONT_ANNOUNCE_BAD);
                        soundManager.add(SND_GAMEOVER, 0);
                    } else {
                        if (var.transition_half_done) {
                            var.titleScreenShow = true;
                            effectMananger.kill(var.effectnum);
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
                    soundManager.add(SND_NEXTLEVEL, 0);

                    if (ballManager.activeBalls > 1) {
                        sprintf(txt, "Bonus: %i", ballManager.activeBalls * 150);
                        player.score += (ballManager.activeBalls * 150) * player.multiply;
                        announce.write(txt, 2000, FONT_ANNOUNCE_GOOD);
                    }

                    effectMananger.set(FX_VAR_TYPE, FX_TRANSIT);
                    effectMananger.set(FX_VAR_LIFE, 1600);
                    effectMananger.set(FX_VAR_COLOR, 0.0, 0.0, 0.0);
                    p.x = 0.0;
                    p.y = 0.0;

                    // Kør en transition effekt
                    var.effectnum = effectMananger.spawn(p);
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
                                announce.write("Finished!", 3500, FONT_ANNOUNCE_GOOD);
                            }

                            sprintf(txt, "Level %i", player.level + 1); // +1 fordi levels starter fra 0
                            announce.write(txt, 1500, FONT_ANNOUNCE_GOOD);

                            //check om vi skal fjerne powerups
                            if (player.difficulty > EASY) {
                                resetPlayerPowerups();
                            }
                            gVar.newLevel = true;
                            var.paused = false;
                        }
                    }

                    if (!effectMananger.isActive(var.effectnum)) {
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
                // Level-Manager code
                //set_up_bricks_for_level(player.level, bricks, texLvl);
                load_level(levelFile, bricks, player.level);
                init_levels(bricks, texLvl);
                gVar.gameOver = false;
                gVar.newLife = true;
                powerupManager.clear();
                bullet.clear();
                paddle.pos_x = 0.0;
                var.startedPlaying = false;
                background.updateBgIfNeeded(textureManager);
                hud.clearShop();
            }

            if (gVar.newLife) {
                gVar.newLife = false;
                paddle.init();
                p.x = paddle.pos_x;
                p.y = paddle.pos_y + paddle.height + 0.025f;

                ballManager.clear();
                ballManager.spawn(p, true, paddle.width, runtime_difficulty.ballspeed[player.difficulty], 1.57100000f);
                // Not exactly degree 90, so the ball will always fall a bit to the side
            }

            if (frameAge >= maxFrameAge) {
                if (var.clearScreen) {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }
                if (setting.showBg)
                    background.draw();
                score.update(player.score);
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
                        ballManager.checkBallCollision(bricks[i], effectMananger);
                        //bullets
                        if (player.powerup[PO_GUN]) {
                            bullet.coldet(bricks[i], effectMananger);
                        }
                        //check kollision på effekterne
                        if (setting.particleCollide && setting.eyeCandy && frameAge >= maxFrameAge)
                            effectMananger.coldet(bricks[i]);
                    }
                    if (frameAge >= maxFrameAge) {
                        bricks[i].draw(bricks, effectMananger);
                    }
                } //aktiv brik
            } // for loop

            // Collision between paddle and balls
            if (ballManager.checkPaddleCollision(paddle, effectMananger)) {
                if (player.powerup[PO_DROP]) {
                    updateBrickPositions(bricks);
                }
            }
            ballManager.move();

            if (setting.particleCollide && setting.eyeCandy && frameAge >= maxFrameAge)
                effectMananger.pcoldet(paddle);

            powerupManager.move();
            if (powerupManager.coldet(paddle, effectMananger, ballManager)) {
                if (player.powerup[PO_DETONATE]) {
                    player.powerup[PO_DETONATE] = false;
                    detonateExplosives(bricks, effectMananger);
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
                soundManager.play();

                if (player.explodePaddle) {
                    player.explodePaddle = false;
                    soundManager.add(SND_DIE, 0);
                    if (setting.eyeCandy) {
                        effectMananger.set(FX_VAR_TYPE, FX_PARTICLEFIELD);

                        p.x = paddle.width * 2;
                        p.y = paddle.height * 2;
                        effectMananger.set(FX_VAR_RECTANGLE, p);

                        p.x = paddle.pos_x;
                        p.y = paddle.pos_y;

                        effectMananger.set(FX_VAR_LIFE, 2000);
                        effectMananger.set(FX_VAR_NUM, 20);
                        effectMananger.set(FX_VAR_SIZE, 0.025f);
                        effectMananger.set(FX_VAR_SPEED, 0.35f);
                        effectMananger.set(FX_VAR_GRAVITY, -0.7f);
                        effectMananger.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
                        effectMananger.spawn(p);
                        effectMananger.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
                        effectMananger.spawn(p);
                        effectMananger.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
                        effectMananger.spawn(p);
                        effectMananger.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
                        effectMananger.spawn(p);

                        effectMananger.set(FX_VAR_NUM, 32);
                        effectMananger.set(FX_VAR_SIZE, 0.05f);
                        effectMananger.set(FX_VAR_LIFE, 1500);
                        effectMananger.set(FX_VAR_SPEED, 0.7f);
                        effectMananger.set(FX_VAR_GRAVITY, 0.0f);

                        effectMananger.set(FX_VAR_COLOR, 0.5f, 0.5f, 0.5f);
                        effectMananger.spawn(p);
                        effectMananger.set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f);
                        effectMananger.spawn(p);
                    }
                }

                ballManager.updateLast();
                glColor3d(255, 255, 255);
                bullet.draw();
                paddle.draw();
                powerupManager.draw();
                ballManager.draw(paddle);
                score.draw();
                speedo.draw();
                hud.draw();
                effectMananger.draw();

                if (var.showHighScores)
                    hKeeper.draw();

                if (var.menuShown > 0) {
                    if (var.menuShown == 10 || var.menuShown == 11) {
                        controller.calibrate();
                    }
                    menu.doMenu();
                }

                announce.draw();
#if DEBUG_SHOW_MOUSE_COORDINATES
#include "DebugMouse.cpp"
                debugMouse.update(normalizedMouseX, normalizedMouseY);
#endif
                SDL_GL_SwapWindow(display.sdlWindow);
                frameAge = 0;
                globalTicksSinceLastDraw = 0;
                globalMilliTicksSinceLastDraw = 0;
            }

            if (!gVar.bricksleft) {
                gVar.nextlevel = true;
                var.paused = true;
            }
        } else {
            titleScreen.draw(&frameAge, &maxFrameAge);
        }
#pragma endregion update & rendering
        usleep(1000);
    }
    return EXIT_SUCCESS;
}

void makeExplosive(brick &b) {
    if (b.type != 'B') {
        b.type = 'B';
        b.texture = *texExplosiveBrick;
        // NOTE: for some reason, the color of the object was changed, why??
        b.justBecomeExplosive = true;
    }
}
