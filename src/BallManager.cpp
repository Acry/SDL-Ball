#include "game_state.h"
#include "MovingObject.h"
#include "Tracer.h"
#include "Paddle.h"
#include "Ball.h"
#include "MathHelper.h"

class BallManager {
    // verwaltet mehrere Bälle
public:
    int activeBalls;
    Ball b[MAXBALLS];
    SpriteSheetAnimation tex[3];

    void initBalls() {
        activeBalls = 0;
        clear();
    }

    BallManager(SpriteSheetAnimation btex[]) {
        tex[0] = btex[0];
        tex[1] = btex[1];
        tex[2] = btex[2];


        for (auto &i: b) {
            i.texture = tex[0];
            i.fireTex = tex[1];
            i.tracer.tex = &tex[2];
        }

        initBalls();
    }

    void getSpeed() {
        var.averageBallSpeed = 0.0;
        for (auto &i: b) {
            if (i.active) {
                var.averageBallSpeed += i.velocity;
            }
        }
        var.averageBallSpeed /= activeBalls;
    }

    // klon alle aktive bolde
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
                position op;
                c++;
                op.y = b[i].pos_y;
                op.x = b[i].pos_x;
                spawn(op, false, 0.0f, b[i].velocity, random_float(BALL_MAX_DEGREE + BALL_MIN_DEGREE, 0));
            }
        }
    }

    void unglue() {
        for (auto &i: b) {
            i.glued = false;
        }
    }

    void spawn(position p, bool glued, GLfloat gx, GLfloat speed, GLfloat angle) {
        for (auto &i: b) {
            if (!i.active) {
                activeBalls++;
                i.texture = tex[0];
                i.fireTex = tex[1];
                i.glued = glued;

                i.width = 0.0;
                i.height = 0.0;
                i.gluedX = gx;
                i.active = true;
                i.collide = true;
                i.reflect = true;
                i.lastX = p.x;
                i.lastY = p.y;
                i.pos_x = p.x;
                i.pos_y = p.y;
                i.explosive = false;
                i.setSpeed(speed);
                i.setAngle(angle);
                i.setSize(0.025);

                //New balls get already applied powerups if not hard
                if (player.difficulty < HARD) {
                    i.explosive = player.powerup[PO_EXPLOSIVE];

                    if (player.powerup[PO_SMALLBALL]) {
                        applyPowerup(PO_SMALLBALL);
                    }

                    if (player.powerup[PO_BIGBALL]) {
                        applyPowerup(PO_BIGBALL);
                    }
                }
                getSpeed();
                break;
            }
        }
    }

    void clear() {
        activeBalls = 0;
        for (auto &i: b) {
            i.active = false;
        }
        getSpeed();
    }

    void move() {
        int a = 0;

        for (auto &i: b) {
            if (i.active) {
                i.move();
                a++;
            }
        }
        activeBalls = a;
    }

    void draw(const Paddle &paddle) {
        for (auto &i: b) {
            if (i.active) {
                i.draw(paddle);
            }
        }
    }

    void checkBallCollision(brick &bri, EffectManager &fxMan) {
        position p;
        for (auto &i: b) {
            if (i.active) {
                p.x = 100;
                collision_ball_brick(bri, i, p, fxMan);
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

    int checkPaddleCollision(const Paddle &paddle, EffectManager &fxMan) {
        int hits = 0;
        position p;
        for (auto &i: b) {
            if (i.active) {
                if (i.glued)
                    i.pos_x = paddle.pos_x + paddle.width - i.gluedX;

                p.x = 100;
                i.checkPaddleCollision(i, paddle, p);
                if (p.x < 50) {
                    hits++;
                    getSpeed();

                    if (player.powerup[PO_GLUE]) {
                        soundManager.add(SND_GLUE_BALL_HIT_PADDLE, p.x);
                    } else {
                        soundManager.add(SND_BALL_HIT_PADDLE, p.x);
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
                        p.y = paddle.pos_y + paddle.height;
                        fxMan.set(FX_VAR_SPEED, 0.5f);
                        fxMan.spawn(p);
                    } //eyecandy
                } // if col
            } //if active
        } //for loop
        return (hits);
    }

    void updateLast() {
        for (auto &i: b) {
            if (i.active) {
                i.lastX = i.pos_x;
                i.lastY = i.pos_y;
            }
        }
    }

    void applyPowerup(const int powerup) {
        for (auto &i: b) {
            if (i.active) {
                switch (powerup) {
                    case PO_BIGBALL: // big balls
                        i.setSize(0.04);
                        i.setSpeed(runtime_difficulty.ballspeed[player.difficulty]);
                        break;
                    case PO_SMALLBALL: // smaller balls
                        // half size
                        i.setSize(0.015);
                        // speed up ball?
                        i.setSpeed(
                            i.velocity + ((i.velocity / 100.f) * runtime_difficulty.speedup[player.difficulty]));
                        break;
                    case PO_NORMALBALL: // normal balls
                        i.setSize(0.025);
                        i.setSpeed(runtime_difficulty.ballspeed[player.difficulty]);
                        break;
                    case PO_EXPLOSIVE: // explosive balls
                        i.explosive = true;
                        i.tracer.colorRotate(true, nullptr);
                        break;
                    default: ;
                }
            }
        }
        getSpeed();
    }
};
