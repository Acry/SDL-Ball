#include "Texture.h"
class HighScore {
    string name;
    TextureManager texMgr;
    Texture tex;
    TtfLegacyGl& text;  // Als Referenz
public:
    HighScore()  : text(TtfLegacyGl::getInstance())
    {
        name = "";
        texMgr.load(themeManager.getThemeFilePath("gfx/highscore/entername.png", setting.gfxTheme), tex);
    }

    void draw() const {
        glColor4f(GL_WHITE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 0.5f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 0.5f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, -0.5f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, -0.5f, 0.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        text.write(name, FONT_MENUHIGHSCORE, true, 2.0, 0.0, 0.2);
    }

    void type(const SDL_Event &e, Menu &menu) {
        if (var.showHighScores == 1) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                var.showHighScores = 0;
                initNewGame();
                menu.refreshHighScoreList();
                var.menu = 7;
                return;
            }

            if (e.key.keysym.sym != SDLK_RETURN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    if (!name.empty())
                        name.erase(name.length() - 1);
                } else {
                    name += e.key.keysym.sym;
                }
            } else {
                var.showHighScores = 0;
                ofstream hsList;
                hsList.open(configFileManager.getHighScoreFile().data(), ios::out | ios::app);
                hsList << "[" << player.level + 1 << "]" << player.score << "|" << name << endl;
                hsList.close();
                initNewGame();
                menu.refreshHighScoreList();
                resumeGame();
                var.titleScreenShow = true;
            }
        }
    }

    static bool isHighScore() {
        int n;
        int high = 0;
        const score *slist = sortScores(&n);
        for (int i = 0; i < n; i++) {
            if (player.score < slist[i].points) {
                high++;
            }
        }
        if (n > 0)
            delete[] slist;

        if (high < 20) {
            return true;
        }
        return false;
    }
};
