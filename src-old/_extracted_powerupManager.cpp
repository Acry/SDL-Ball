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
