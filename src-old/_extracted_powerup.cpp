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