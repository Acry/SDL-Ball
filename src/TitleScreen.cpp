#include "Texture.h"

class powerupDescriptionClass : public MovingObject {
    Text& text;
public:
    powerupDescriptionClass();
    Texture *tex;

    void draw() const;

    string name;
    string description;
};

powerupDescriptionClass::powerupDescriptionClass() : text(Text::getInstance()) {
    // Icon size
    width = 0.035;
    height = 0.035;
}

void powerupDescriptionClass::draw() const {
    // draw powerup icon, name and description
    tex->play();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->textureProperties.texture);
    glColor4f(tex->textureProperties.glTexColorInfo[0], tex->textureProperties.glTexColorInfo[1], tex->textureProperties.glTexColorInfo[2],
              tex->textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(tex->texturePosition[0], tex->texturePosition[1]);
    glVertex3f(-width + posx, height + posy, 0.00); // top left
    glTexCoord2f(tex->texturePosition[2], tex->texturePosition[3]);
    glVertex3f(width + posx, height + posy, 0.00); // top right
    glTexCoord2f(tex->texturePosition[4], tex->texturePosition[5]);
    glVertex3f(width + posx, -height + posy, 0.00); // bottom right
    glTexCoord2f(tex->texturePosition[6], tex->texturePosition[7]);
    glVertex3f(-width + posx, -height + posy, 0.00); // bottom left
    glEnd();
    glDisable(GL_TEXTURE_2D);
    constexpr float spacing = 0.05f;
    // Write the name and description
    constexpr float scale = 1.0f; // Scale for the text
    constexpr float leading = -0.018f; // Leading for the text
    const float textX = posx + spacing;
    text.write(name,
                  FONT_INTRODESCRIPTION,
                  false,
                  scale, textX,
                  posy - leading);
    text.write(description,
                  FONT_INTRODESCRIPTION,
                  false,
                  scale,
                  textX,
                  posy - height - leading);
}

class TitleScreen {
    Text& text;  // Als Referenz
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    TextureManager texMgr;
    Texture texTitle;
    Texture *texPowerups;
    GLuint glTitleList;
    float rot;
    bool rotDir;
    powerupDescriptionClass powerUp[MAXPOTEXTURES];
    int numHighScores; //Number of highscores to show in the intro
    pos runnerPos;
    Menu *menu; //Here is the highscore text
    int runnerTime;
    float runnerVelX, runnerVelY;
    int hilight;
    bool hilightDir;
    int hilightTime;

    static void readDescriptions(powerupDescriptionClass po[]);

public:
    TitleScreen(effectManager *m, Texture tp[], Menu *me);

    void draw(Uint32 *frame_age, Uint32 *max_frame_age);
};

TitleScreen::TitleScreen(effectManager *m, Texture tp[], Menu *me): text(Text::getInstance())
{
    menu = me;
    numHighScores = 7;
    texPowerups = tp;
    fxMan = m;
    ticksSinceLastSpawn = 100;
    // SDL-BALL
    texMgr.load(themeManager.getThemeFilePath("gfx/title/title.png", setting.gfxTheme), texTitle);

    glTitleList = glGenLists(1);
    glNewList(glTitleList, GL_COMPILE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTitle.textureProperties.texture);
    glBegin(GL_QUADS);
    for (int i = 0; i < 32; i++) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.005f * i);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 1.15f, 0.005f * i);
        glColor4f(0.0f, 0.0f, 1.0f, 0.00); // ???
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 0.75f, 0.005f * i);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 0.75f, 0.005f * i);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEndList();

    // set position for powerup icons
    constexpr float iconHeight = 0.035;
    constexpr float iconWidth = 0.035;
    constexpr float spacing = 0.45f; // Abstand zwischen Spalten
    constexpr float leading = 0.09; // Abstand zwischen Zeilen
    constexpr float totalWidth = (iconWidth + spacing * 3); // Gesamtbreite mit Text
    constexpr float startX = -totalWidth / 2.0f;
    constexpr float startY = -0.2f;

    for (int row = 0; row < 7; row++) {
        for (int column = 0; column < 3; column++) {
            int idx = column + 3 * row;
            powerUp[idx].tex = &texPowerups[idx];
            powerUp[idx].posx = startX + column * (iconWidth + spacing);
            powerUp[idx].posy = startY - (leading + iconHeight) * row;
        }
    }

    readDescriptions(powerUp);

    // Set the initial position and velocity of the runner
    runnerPos.x = 0.0;
    runnerPos.y = 0.66;
    runnerVelX = random_float(2, 1) + 2;
    runnerVelY = random_float(2, 1) + 2;
    runnerTime = 0;

    hilight = 0;
    hilightDir = true;
    hilightTime = 0;
}

void TitleScreen::draw(Uint32 *frameAge, Uint32 *maxFrameAge) {
    if (*frameAge >= *maxFrameAge) {
        soundManager.play();
        glLoadIdentity();
        if (var.clearScreen) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        // icons and text
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

        for (i = 0; i < MAXPOTEXTURES; i++) {
            powerUp[i].draw();
        }

        // runner
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

            if (runnerPos.x > 1.0f && runnerVelX > 0) {
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

        // hilight highscores
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

        for (i = 0; i < numHighScores; i++) {
            if ((hilightDir && i < hilight + 1) || (!hilightDir && i > hilight - 1)) {
                const float a = 1.0 - 1.0 / static_cast<float>(numHighScores * 2) * std::abs(hilight - i);
                glColor4f(0.9f, 0.9f, 0.9f, a);
                // SDL_Log("alpha: %f", a);
                text.write(menu->highScores[i], FONT_INTROHIGHSCORE, true, 0.5, 0.0, 0.063 * i);
            }
        }

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

        glTranslatef(0.0, -0.05, 0.0);
        glRotatef(20, 1, 0, 0);
        glRotatef(rot, 0, 1, 0);
        glCallList(glTitleList);

        SDL_GL_SwapWindow(display.sdlWindow);

        globalTicksSinceLastDraw = 0;
        globalMilliTicksSinceLastDraw = 0;
        *frameAge = 0;
    }
}

void TitleScreen::readDescriptions(powerupDescriptionClass po[]) {
    ifstream f;

    f.open(themeManager.getThemeFilePath("/powerupdescriptions.txt", setting.gfxTheme).data());
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
        SDL_Log("Could not open powerup descriptions");
    }
}
