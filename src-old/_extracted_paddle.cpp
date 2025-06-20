class paddle_class : public object {
private:
  GLfloat growspeed;
  GLfloat destwidth;
  GLfloat aspect; //så meget stiger højden i forhold til bredden
  bool growing;

public:
  bool dead;
  textureClass *layerTex;

  paddle_class() {
    init();
    growing = 0;
    growspeed = 0.05f;
    aspect = 0.2f;
  }

  void init() {
    posy = -1.15;
    width = 0.1;
    height = 0.02;
    dead = 0;
  }

  void grow(GLfloat width) {
    destwidth = width;
    growing = 1;
  }

  void draw() {
    if (!dead) {
      if (player.powerup[PO_DIE]) {
        player.powerup[PO_DIE] = 0;
        dead = 1;
        width = 0.0;
        height = 0.0;
      }

      if (growing) {
        GLfloat ix = growspeed * globalTicksSinceLastDraw;

        width += ix;

        if (width >= destwidth) {
          width = destwidth;
          height = aspect * destwidth;
          growing = 0;
        }
      }

      glLoadIdentity();
      glTranslatef(posx, posy, -3.0);

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
      glEnd();

      //Hvis glue?
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
        glTexCoord2f(layerTex[1].pos[4], layerTex[1].pos[5] - 0.01);
        glVertex3f(width, height, 0.0f);
        glTexCoord2f(layerTex[1].pos[6], layerTex[1].pos[7] - 0.01);
        glVertex3f(-width, height, 0.0f);
        glEnd();
      }
    }
  }
};
