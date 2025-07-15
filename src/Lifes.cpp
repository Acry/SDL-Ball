// Lifes.cpp
#include "Lifes.h"

Lifes::Lifes(TextureManager *textureManager, int initialLives)
    : textureManager(textureManager), lives(initialLives) {
}

void Lifes::addLives(const int delta) {
    lives = std::max(0, lives + delta);
}

int Lifes::getLives() const {
    return lives;
}

void Lifes::setLives(const int value) {
    lives = std::max(0, value);
}

void Lifes::draw() const {
    if (!textureManager) return;
    const TextureResource textureProperties = textureManager->getBallTexture(BallTexture::Normal)->textureProperties;

    constexpr float ballSize = 0.04f;
    constexpr float spacing = 0.005f;

    // Startposition
    constexpr float startX = 0.95f - ballSize - spacing;
    constexpr float startY = 0.925f - ballSize - spacing;

    for (int i = 0; i < lives; ++i) {
        constexpr int iconsPerRow = 10;
        const int row = i / iconsPerRow;
        const int col = i % iconsPerRow;

        const float pos_x = startX - col * (ballSize + spacing);
        const float pos_y = startY - row * (ballSize + spacing);

        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, textureProperties.id);
        glColor4fv(textureProperties.textureColor);
        glBegin(GL_QUADS);

        // Bottom-left
        glTexCoord2f(textureProperties.uvCoordinates[0], textureProperties.uvCoordinates[1]);
        glVertex3f(pos_x, pos_y, 0.0f);

        // Bottom-right
        glTexCoord2f(textureProperties.uvCoordinates[2], textureProperties.uvCoordinates[3]);
        glVertex3f(pos_x + ballSize, pos_y, 0.0f);

        // Top-right
        glTexCoord2f(textureProperties.uvCoordinates[4], textureProperties.uvCoordinates[5]);
        glVertex3f(pos_x + ballSize, pos_y + ballSize, 0.0f);

        // Top-left
        glTexCoord2f(textureProperties.uvCoordinates[6], textureProperties.uvCoordinates[7]);
        glVertex3f(pos_x, pos_y + ballSize, 0.0f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
}
