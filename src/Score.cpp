// Score.cpp
#include "Score.h"
#include "TextManager.h"

Score::Score(TextManager &textManager)
    : text(textManager),
      timeSinceLastUpdate(0.0f),
      tempScore(0),
      score(0) {
    tempText[0] = '0';
    tempText[1] = '\0';
}

void Score::reset() {
    tempScore = 0;
    score = 0;
    tempText[0] = '0';
    tempText[1] = '\0';
    timeSinceLastUpdate = 0.0f;
}

void Score::update(const float deltaTime) {
    const int diff = score - tempScore;
    if (tempScore != score) {
        // Akkumuliere die Zeit seit dem letzten Update
        timeSinceLastUpdate += deltaTime;

        // Update alle 50ms (0.05 Sekunden)
        if (timeSinceLastUpdate >= 0.05f) {
            if (diff > 0)
                tempScore += static_cast<float>(diff) / 7.0f + 1.0f;
            else
                tempScore += static_cast<float>(diff) / 7.0f - 1.0f;

            // Begrenzung, damit tempScore nicht über das Ziel hinausläuft
            if ((diff > 0 && tempScore > score) || (diff < 0 && tempScore < score))
                tempScore = score;

            timeSinceLastUpdate = 0.0f;
            sprintf(tempText, "%i", tempScore);
        }
    }
}


void Score::draw() const {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    text.write(tempText, Fonts::Highscore, false, 0.75f, -0.94f, 0.97f);
}

void Score::addPoints(const int points) {
    score += points;
}
