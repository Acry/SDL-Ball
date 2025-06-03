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
