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
    int i;
    tex[0] = btex[0];
    tex[1] = btex[1];
    tex[2] = btex[2];


    for (i = 0; i < MAXBALLS; i++) {
      b[i].tex = tex[0];
      b[i].fireTex = tex[1];
      b[i].tail.tex = &tex[2];
    }

    initBalls();
  }

  void getSpeed() {
    int i;
    var.averageBallSpeed = 0.0;
    for (i = 0; i < MAXBALLS; i++) {
      if (b[i].active) {
        var.averageBallSpeed += b[i].velocity;
      }
    }
    var.averageBallSpeed /= activeBalls;
  }

  //klon alle aktive bolde
  void multiply() {
    pos op;
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
        c++;
        op.y = b[i].posy;
        op.x = b[i].posx;
        spawn(op, 0, 0.0f, b[i].velocity, rndflt(BALL_MAX_DEGREE + BALL_MIN_DEGREE, 0));
      }
    }
  }

  void unglue() {
    int i;
    for (i = 0; i < MAXBALLS; i++) {
      b[i].glued = 0;
    }
  }

  void spawn(pos p, bool glued, GLfloat gx, GLfloat speed, GLfloat angle) {
    int i;
    for (i = 0; i < MAXBALLS; i++) {
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
    int i;
    activeBalls = 0;
    for (i = 0; i < MAXBALLS; i++) {
      b[i].active = 0;
    }
    getSpeed();
  }

  void move() {
    int a = 0;
    int i;

    for (i = 0; i < MAXBALLS; i++) {
      if (b[i].active) {
        b[i].move();
        a++;
      }
    }
    activeBalls = a;
  }

  void draw(paddle_class &paddle) {
    int i;

    for (i = 0; i < MAXBALLS; i++) {
      if (b[i].active) {
        b[i].draw(paddle);
      }
    }
  }

  void bcoldet(brick &bri, effectManager &fxMan) {
    int i;
    pos p;
    for (i = 0; i < MAXBALLS; i++) {
      if (b[i].active) {
        p.x = 100;
        coldet(bri, b[i], p, fxMan);
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
    int i, hits = 0;
    pos p;
    for (i = 0; i < MAXBALLS; i++) {
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
    int i;
    for (i = 0; i < MAXBALLS; i++) {
      if (b[i].active) {
        b[i].lastX = b[i].posx;
        b[i].lastY = b[i].posy;
      }
    }
  }

  void powerup(int powerup) {
    int i;
    for (i = 0; i < MAXBALLS; i++) {
      if (b[i].active) {
        switch (powerup) {
          case PO_BIGBALL: //big balls
            b[i].setSize(0.04);
            b[i].setspeed(difficulty.ballspeed[player.difficulty]);
            break;
          case PO_SMALLBALL: //small balls
            b[i].setSize(0.015);
            //speed bolden op
            b[i].setspeed(b[i].velocity + ((b[i].velocity / 100.f) * difficulty.speedup[player.difficulty]));
            break;
          case PO_NORMALBALL: // normal balls
            b[i].setSize(0.025);
            b[i].setspeed(difficulty.ballspeed[player.difficulty]);
            break;
          case PO_EXPLOSIVE: //exploderer brikker
            b[i].explosive = 1;
            b[i].tail.colorRotate(TRUE, 0);
            break;
        }
      }
    }

    getSpeed();
  }
};

float bounceOffAngle(GLfloat width, GLfloat posx, GLfloat hitx) {
  return ((BALL_MAX_DEGREE / (width * 2.0)) * (posx + width - hitx) + BALL_MIN_DEGREE);
}

