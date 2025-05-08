class backgroundClass {
    GLfloat r[4], g[4], b[4], a;

    textureClass tex;
    bool firstTexture;

public:
    backgroundClass() {
        firstTexture = true;
    }

    void init(textureManager &texMgr) {
        bool l;
        // Don't delete texture 0 the first time the function is called
        if (!firstTexture)
            glDeleteTextures(1, &tex.prop.texture);

        if (const float t = 0.25 * (static_cast<float>(player.level) + 1); t <= 1.0)
            l = texMgr.load(useTheme("/gfx/bg/1.jpg", setting.gfxTheme), tex);
        else if (t <= 2.0)
            l = texMgr.load(useTheme("/gfx/bg/2.jpg", setting.gfxTheme), tex);
        else if (t <= 3.0)
            l = texMgr.load(useTheme("/gfx/bg/3.jpg", setting.gfxTheme), tex);
        else if (t <= 4.0)
            l = texMgr.load(useTheme("/gfx/bg/4.jpg", setting.gfxTheme), tex);
        else if (t <= 5.0)
            l = texMgr.load(useTheme("/gfx/bg/5.jpg", setting.gfxTheme), tex);
        else if (t <= 6.0)
            l = texMgr.load(useTheme("/gfx/bg/6.jpg", setting.gfxTheme), tex);
        else if (t <= 7.0)
            l = texMgr.load(useTheme("/gfx/bg/7.jpg", setting.gfxTheme), tex);
        else if (t <= 8.0)
            l = texMgr.load(useTheme("/gfx/bg/8.jpg", setting.gfxTheme), tex);
        else if (t <= 9.0)
            l = texMgr.load(useTheme("/gfx/bg/9.jpg", setting.gfxTheme), tex);
        else if (t <= 10.0)
            l = texMgr.load(useTheme("/gfx/bg/10.jpg", setting.gfxTheme), tex);
        else if (t <= 11.0)
            l = texMgr.load(useTheme("/gfx/bg/11.jpg", setting.gfxTheme), tex);
        else if (t <= 12.0)
            l = texMgr.load(useTheme("/gfx/bg/12.jpg", setting.gfxTheme), tex);
        else if (t <= 13.0)
            l = texMgr.load(useTheme("/gfx/bg/13.jpg", setting.gfxTheme), tex);
        else if (t <= 14.0)
            l = texMgr.load(useTheme("/gfx/bg/14.jpg", setting.gfxTheme), tex);
        else if (t <= 15.0)
            l = texMgr.load(useTheme("/gfx/bg/15.jpg", setting.gfxTheme), tex);
        else if (t <= 16.0)
            l = texMgr.load(useTheme("/gfx/bg/16.jpg", setting.gfxTheme), tex);
        else if (t <= 17.0)
            l = texMgr.load(useTheme("/gfx/bg/17.jpg", setting.gfxTheme), tex);
        else if (t <= 18.0)
            l = texMgr.load(useTheme("/gfx/bg/18.jpg", setting.gfxTheme), tex);
        else if (t <= 19.0)
            l = texMgr.load(useTheme("/gfx/bg/19.jpg", setting.gfxTheme), tex);
        else if (t <= 20.0)
            l = texMgr.load(useTheme("/gfx/bg/20.jpg", setting.gfxTheme), tex);
        else if (t <= 21.0)
            l = texMgr.load(useTheme("/gfx/bg/21.jpg", setting.gfxTheme), tex);
        else if (t <= 22.0)
            l = texMgr.load(useTheme("/gfx/bg/22.jpg", setting.gfxTheme), tex);
        else if (t <= 23.0)
            l = texMgr.load(useTheme("/gfx/bg/23.jpg", setting.gfxTheme), tex);
        else if (t <= 24.0)
            l = texMgr.load(useTheme("/gfx/bg/24.jpg", setting.gfxTheme), tex);
        else if (t <= 25.0)
            l = texMgr.load(useTheme("/gfx/bg/25.jpg", setting.gfxTheme), tex);
        else
            l = texMgr.load(useTheme("/gfx/bg/final.jpg", setting.gfxTheme), tex);

        if (!l) {
            setting.showBg = false;
            SDL_Log("Backgrounds disabled.");
        }

        firstTexture = false;

        for (int i = 0; i < 4; i++) {
            r[i] = random_float(1, 0);
            g[i] = random_float(1, 0);
            b[i] = random_float(1, 0);
        }
        a = 1;
    }

    void draw() const {
        glLoadIdentity();
        glTranslatef(0, 0, 0);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex.prop.texture);

        glBegin(GL_QUADS);
        glColor4f(r[0], g[0], b[0], a);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.66, 1.25, 0.0f);
        glColor4f(r[1], g[1], b[1], a);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.66, 1.25, 0.0f);
        glColor4f(r[2], g[2], b[2], a);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.66, -1.25, 0.0f);
        glColor4f(r[3], g[3], b[3], a);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.66, -1.25, 0.0f);
        glEnd();
    }
};
