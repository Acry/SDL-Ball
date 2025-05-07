class powerupDescriptionClass : public moving_object {
public:
    powerupDescriptionClass();

    textureClass *tex;

    void draw() const;

    string name;
    string description;
};

powerupDescriptionClass::powerupDescriptionClass() {
    width = 0.055;
    height = 0.055;
}

void powerupDescriptionClass::draw() const {
    tex->play();
    glBindTexture(GL_TEXTURE_2D, tex->prop.texture);
    glColor4f(tex->prop.glTexColorInfo[0], tex->prop.glTexColorInfo[1], tex->prop.glTexColorInfo[2],
              tex->prop.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(tex->pos[0], tex->pos[1]);
    glVertex3f(-width + posx, height + posy, 0.00); // top left
    glTexCoord2f(tex->pos[2], tex->pos[3]);
    glVertex3f(width + posx, height + posy, 0.00); // top right
    glTexCoord2f(tex->pos[4], tex->pos[5]);
    glVertex3f(width + posx, -height + posy, 0.00); // bottom right
    glTexCoord2f(tex->pos[6], tex->pos[7]);
    glVertex3f(-width + posx, -height + posy, 0.00); // bottom left
    glEnd();

    glColor4f(1.0, 1.0, 1.0, 1.0);

    constexpr float spacing = 0.05f;
    // Write the name and description
    glText->write(name, FONT_INTRODESCRIPTION, false, 1.0, posx + width + spacing,
                  posy + (glText->getHeight(FONT_INTRODESCRIPTION) / 2.0));
    glText->write(description, FONT_INTRODESCRIPTION, false, 1.0, posx + width + spacing,
                  posy - (glText->getHeight(FONT_INTRODESCRIPTION) / 2.0));
}

class titleScreenClass {
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    textureManager texMgr;
    textureClass texTitle;
    textureClass *texPowerups;
    GLuint glTitleList;
    float rot;
    bool rotDir;
    powerupDescriptionClass powerUp[MAXPOTEXTURES];
    int numHighScores; //Number of highscores to show in the intro
    pos runnerPos;
    menuClass *menu; //Here is the highscore text
    int runnerTime;
    float runnerVelX, runnerVelY;
    int hilight;
    bool hilightDir;
    int hilightTime;

    static void readDescriptions(powerupDescriptionClass po[]);

public:
    titleScreenClass(effectManager *m, textureClass tp[], menuClass *me);

    void draw(Uint32 *frame_age, Uint32 *max_frame_age);
};

titleScreenClass::titleScreenClass(effectManager *m, textureClass tp[], menuClass *me) {
    menu = me;
    numHighScores = 7;
    texPowerups = tp;
    fxMan = m;
    ticksSinceLastSpawn = 100;
    texMgr.load(useTheme("/gfx/title/title.png", setting.gfxTheme), texTitle);
    glTitleList = glGenLists(1);
    glNewList(glTitleList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, texTitle.prop.texture);
    glBegin(GL_QUADS);
    for (int i = 0; i < 32; i++) {
        glColor4f(1, 1, 1, 0.1);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-1.2, 1.15, 0.005 * i);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(1.2, 1.15, 0.005 * i);
        glColor4f(0, 0, 1, 0.00);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(1.2, 0.75, 0.005 * i);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-1.2, 0.75, 0.005 * i);
    }
    glEnd();
    glEndList();

    float iconWidth = 0.055;      // Halbe Breite des Icons
    float textWidth = 0.8;        // Platz für Text rechts daneben
    float spacing = 0.1;          // Abstand zwischen Spalten
    float totalWidth = (iconWidth + textWidth) * 3 + spacing * 2; // Gesamtbreite mit Text
    float startX = -(totalWidth/2);

    for(int column = 0; column < 3; column++) {
        for(int row=0; row < 7; row++) {
            powerUp[row+7*column].tex = &texPowerups[row+7*column];
            powerUp[row+7*column].posx = startX + column * (iconWidth + textWidth + spacing);
            powerUp[row+7*column].posy = -0.25 - 0.135 * row;
        }
    }
    readDescriptions(powerUp);

    runnerPos.x = 0.0;
    runnerPos.y = 0.66;
    runnerVelX = random_float(2, 1) + 2;
    runnerVelY = random_float(2, 1) + 2;
    runnerTime = 0;

    hilight = 0;
    hilightDir = true;
    hilightTime = 0;
}

int delta(const int a, const int b) {
    if (a >= b) {
        return a - b;
    }
    return b - a;

}

void titleScreenClass::draw(Uint32 *frameAge, Uint32 *maxFrameAge) {
    if (*frameAge >= *maxFrameAge) {
        soundMan.play();

        if (var.clearScreen) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        glEnable(GL_TEXTURE_2D);

        ticksSinceLastSpawn += globalTicksSinceLastDraw;
        if (ticksSinceLastSpawn > 125) {
            pos s;
            pos p;
            s.x = 3.25;
            s.y = 0.525;
            p.x = 0;
            p.y = 1;

            ticksSinceLastSpawn = 0;
            fxMan->set(FX_VAR_TYPE, FX_PARTICLEFIELD);
            fxMan->set(FX_VAR_COLDET, 0);
            fxMan->set(FX_VAR_LIFE, 1500);
            fxMan->set(FX_VAR_NUM, 50);
            fxMan->set(FX_VAR_SIZE, 0.01f);
            fxMan->set(FX_VAR_SPEED, 0.0f);
            fxMan->set(FX_VAR_GRAVITY, 0.0f);
            fxMan->set(FX_VAR_RECTANGLE, s);
            fxMan->set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f);
            fxMan->spawn(p);
        }
        fxMan->draw();

        int i;
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);

        for (i = 0; i < MAXPOTEXTURES; i++) {
            powerUp[i].draw();
        }


        runnerTime += globalTicksSinceLastDraw;
        if (runnerTime > 10) {
            fxMan->set(FX_VAR_TYPE, FX_SPARKS);
            fxMan->set(FX_VAR_COLDET, 0);
            fxMan->set(FX_VAR_LIFE, 250);
            fxMan->set(FX_VAR_NUM, 2);
            fxMan->set(FX_VAR_SIZE, 0.09f);
            fxMan->set(FX_VAR_SPEED, 0.5f);
            fxMan->set(FX_VAR_GRAVITY, -0.4f);

            fxMan->set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
            fxMan->spawn(runnerPos);
            fxMan->set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
            fxMan->spawn(runnerPos);
            fxMan->set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
            fxMan->spawn(runnerPos);
            fxMan->set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
            fxMan->spawn(runnerPos);

            runnerPos.x += runnerVelX * (runnerTime / 1000.0);
            runnerPos.y += runnerVelY * (runnerTime / 1000.0);

            // Grenzen mit Faktor 3 anpassen
            if (runnerPos.x > 1.0f && runnerVelX > 0) {      // Etwas größer als vorher
                runnerVelX *= -1.0f;
            }
            if (runnerPos.x < -1.0f && runnerVelX < 0) {
                runnerVelX *= -1.0f;
            }
            if (runnerPos.y > 1.0f && runnerVelY > 0) {
                runnerVelY *= -1.0f;
            }
            if (runnerPos.y < -1.0f && runnerVelY < 0) {
                runnerVelY *= -1.0f;
            }
            runnerTime = 0;
        }
        glPopMatrix();

        hilightTime += globalTicksSinceLastDraw;
        if (hilightTime > 50) {
            if (hilightDir) {
                hilight++;
                if (hilight == numHighScores * 3) //-1)
                {
                    hilight = numHighScores - 1;
                    hilightDir = false;
                }
            } else {
                hilight--;
                if (hilight == -numHighScores * 2) {
                    hilightDir = true;
                    hilight = 0;
                }
            }
            hilightTime = 0;
        }
        glPushMatrix();
        glTranslatef(0.0, 0.59, 0.0);
        for (i = 0; i < numHighScores; i++) {
            if ((hilightDir && i < hilight + 1) || (!hilightDir && i > hilight - 1)) {
                const float a = 1.0 - 1.0 / static_cast<float>(numHighScores * 2) * delta(hilight, i);
                glColor4f(1, 1, 1, a);
                glText->write(menu->highScores[i], FONT_INTROHIGHSCORE, 1, 1.0, 0.0, 0.0);
            }
            glTranslatef(0.0, -glText->getHeight(FONT_INTROHIGHSCORE), 0.0);
        }
        glPopMatrix();
        if (!rotDir) {
            rot += 0.01 * globalTicksSinceLastDraw;
            if (rot > 40) {
                rotDir = true;
            }
        } else {
            rot -= 0.01 * globalTicksSinceLastDraw;
            if (rot < -40) {
                rotDir = false;
            }
        }

        glPushMatrix();
        glTranslatef(0.0, -0.05, 0.0);
        glRotatef(20, 1, 0, 0);
        glRotatef(rot, 0, 1, 0);
        glCallList(glTitleList);
        glPopMatrix();
        SDL_GL_SwapWindow(display.sdlWindow);

        globalTicksSinceLastDraw = 0;
        globalMilliTicksSinceLastDraw = 0;
        *frameAge = 0;
    }
}


void titleScreenClass::readDescriptions(powerupDescriptionClass po[]) {
    ifstream f;

    f.open(useTheme("/powerupdescriptions.txt", setting.gfxTheme).data());
    if (f.is_open()) {
        bool flip = false;
        int p = 0;
        string line;
        while (!f.eof()) {
            getline(f, line);
            if (!flip) {
                flip = true;
                po[p].name = line;
            } else {
                flip = false;
                po[p].description = line;
                p++;
            }
            if (p == MAXPOTEXTURES)
                break;
        }
        f.close();
    } else {
        SDL_Log("Could not open powerupdescriptions");
    }
}
