#include "../src/Brick_old.h"
int nbrick[23][26];
int updated_nbrick[23][26];

void makeExplosive(brick & b);
void makeExplosive(brick &b) {
    if (b.type != 'B') {
        b.type = 'B';
        b.texture = *texExplosiveBrick;
        // NOTE: for some reason, the color of the object was changed, why??
        b.justBecomeExplosive = true;
    }
}

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