class ball : public moving_object {
private:
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
    growing = 0;
    growspeed = 0.1;
    width = 0.0;
    height = 0.0;
    glued = 0;
    posx = 0.0f;
    posy = 0.0f;
    aimdir = 0;
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
      active = 0;
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

  void draw(paddle_class &paddle) {
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

        growing = 0;
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

        shrinking = 0;
      }

      tail.width = width;
      tail.height = height;
    }

    if (glued && player.powerup[PO_LASER]) {
      if (player.powerup[PO_AIM]) {
        if (aimdir == 0) {
          rad -= 1.2 * globalMilliTicksSinceLastDraw;

          if (rad < BALL_MIN_DEGREE)
            aimdir = 1;
        } else {
          rad += 1.2 * globalMilliTicksSinceLastDraw;
          if (rad > BALL_MAX_DEGREE + BALL_MIN_DEGREE)
            aimdir = 0;
        }
        setangle(rad);
      } else {
        getRad();
      }

      GLfloat bxb = cos(rad) * 0.5, byb = sin(rad) * 0.5;

      glLoadIdentity();
      glTranslatef(posx, posy, -3.0);
      glDisable(GL_TEXTURE_2D);
      glLineWidth(1.0);
      glEnable(GL_LINE_SMOOTH);
      glBegin(GL_LINES);
      glColor4f(rndflt(2, 0), rndflt(1, 0), 0.0, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glColor4f(rndflt(2, 0), 0.0, 0.0, 1.0);
      glVertex3f(bxb, byb, 0.0);
      glEnd();

      glPointSize(5.0f);
      glColor4f(1.0, 0.0, 0.0, 1.0);
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

      GLfloat cx, cy, R;
      if (LinesCross(p[0], p[2], p[1], p[3], b[0], b[2], b[1], b[3], &cx, &cy)) {
        R = bounceOffAngle(paddle.width, paddle.posx, cx);
        o[0] = cx + (cos(R) * 2.0);
        o[1] = cy + (sin(R) * 2.0);
        glLoadIdentity();
        glTranslatef(0.0, 0.0, -3.0);
        glDisable(GL_TEXTURE_2D);
        glLineWidth(2.0);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        //Line from ball to paddle
        glColor4f(1.0, 0.0, 0.0, 0.0);
        glVertex3f(b[0], b[2], 0.0);
        glColor4f(1.0, 1.0, 0.0, 1.0);
        glVertex3f(cx, cy, 0.0);
        //Bounce off line.
        glColor4f(1.0, 0.0, 0.0, 0.0);
        glVertex3f(o[0], o[1], 0.0);
        glEnd();
      }
    }

    glLoadIdentity();
    glTranslatef(posx, posy, -3.0);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (explosive) {
      fireTex.play();
      glBindTexture(GL_TEXTURE_2D, fireTex.prop.texture);
      glColor4f(fireTex.prop.glTexColorInfo[0], fireTex.prop.glTexColorInfo[1], fireTex.prop.glTexColorInfo[2],
                fireTex.prop.glTexColorInfo[3]);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
      glTranslatef(posx, posy, -3.0);
      glDisable( GL_TEXTURE_2D );
      glColor4f(1.0,1.0,1.0,1.0);
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

void coldet(brick & br, ball & ba, pos & p, effectManager & fxMan);
void padcoldet(ball & b, paddle_class & p, pos & po);
