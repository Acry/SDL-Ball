class glScoreBoard {
    unsigned int lastScoreTick;
    uint tempScore;
    uint score;
    char tempText[255];

public:
    glScoreBoard() {
        init();
    }

    void init() {
        tempScore = 1;
        score = 0;
        lastScoreTick = SDL_GetTicks();
    }

    void update(const int point) {
        score = point;
    }

    void draw() {
        const int dif = score - tempScore;

        if (tempScore != score) {
            if (lastScoreTick + 50 < SDL_GetTicks()) {
                tempScore += static_cast<float>(dif) / 7.0f + 1.0f;;
                if (tempScore > score)
                    tempScore = score;
                lastScoreTick = SDL_GetTicks();

                sprintf(tempText, "%i", tempScore);
            }
        }
        glLoadIdentity();
        glText->write(tempText, FONT_HIGHSCORE, false, 1.0f, -0.9f, 0.9f);
    }
};
