#include <SDL2/SDL.h>
#include <string>
#include <epoxy/gl.h>

#include "MathHelper.h"
#include "TextureManager.h"
#include "BackgroundManager.h"

BackgroundManager::BackgroundManager(const TextureManager &texMgr) : maxLevel(0), backgroundDisplayList(0),
                                                                     showBackgroundOverlay(true) {
    textureManager = &texMgr;

    colors.fill(Color{0.9f, 0.9f, 0.9f, 0.95f});
}

bool BackgroundManager::updateBgIfNeeded(Uint32 level) {
    const size_t bgCount = textureManager->getBackgroundCount();
    if (bgCount == 0) {
        SDL_Log("Error: No backgrounds available in the TextureManager.");
        return false;
    }

    bool textureChanged = false;
    level += 1;

    texture *newTexture = nullptr;

    if (level >= maxLevel) {
        newTexture = textureManager->getBackground(bgCount - 1);
        if (!newTexture) {
            SDL_Log("Error: Could not get final background texture");
            return false;
        }
    } else {
        const size_t regularBgCount = bgCount - 1;
        const size_t levelsPerBg = maxLevel / regularBgCount;
        const size_t bgIndex = (level - 1) / levelsPerBg;
        const size_t safeBgIndex = std::min(bgIndex, regularBgCount - 1);

        newTexture = textureManager->getBackground(safeBgIndex);
        if (!newTexture) {
            SDL_Log("Error: Could not get background texture for index %zu", bgIndex);
            return false;
        }
    }

    textureChanged = (currentTexture == nullptr || currentTexture->textureProperties.id != newTexture->textureProperties
                      .id);

    if (textureChanged || showBackgroundOverlay) {
        currentTexture = newTexture;
        createDisplayList();
    }

    return true;
}

void BackgroundManager::draw() const {
    if (backgroundDisplayList != 0) {
        glCallList(backgroundDisplayList);
    }
}

void BackgroundManager::drawQuad() const {
    if (!currentTexture) return;
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, currentTexture->textureProperties.id);

    glBegin(GL_QUADS);

    // Bottom-left corner.
    if (showBackgroundOverlay)
        glColor4f(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
    glTexCoord2f(currentTexture->textureProperties.uvCoordinates[0],
                 currentTexture->textureProperties.uvCoordinates[1]);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    // Bottom-right corner.
    if (showBackgroundOverlay)
        glColor4f(colors[1].r, colors[1].g, colors[1].b, colors[1].a);
    glTexCoord2f(currentTexture->textureProperties.uvCoordinates[2],
                 currentTexture->textureProperties.uvCoordinates[3]);
    glVertex3f(1.0f, -1.0f, 0.0f);

    // Top-right corner.
    if (showBackgroundOverlay)
        glColor4f(colors[2].r, colors[2].g, colors[2].b, colors[2].a);
    glTexCoord2f(currentTexture->textureProperties.uvCoordinates[4],
                 currentTexture->textureProperties.uvCoordinates[5]);
    glVertex3f(1.0f, 1.0f, 0.0f);

    // Top-left corner.
    if (showBackgroundOverlay)
        glColor4f(colors[3].r, colors[3].g, colors[3].b, colors[3].a);
    glTexCoord2f(currentTexture->textureProperties.uvCoordinates[6],
                 currentTexture->textureProperties.uvCoordinates[7]);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

BackgroundManager::~BackgroundManager() {
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }
}

void BackgroundManager::registerEvents(IEventManager *em) {
    if (!em) return;

    eventManager = em;

    // Level-Change Event
    eventManager->addListener(GameEvent::LevelLoaded, [this](const LevelData &data) {
        this->updateBgIfNeeded(data.currentLevel);
    }, this);

    // Theme-Change Event
    eventManager->addListener(GameEvent::LevelThemeChanged, [this](const LevelThemeData &data) {
        this->maxLevel = data.maxLevel;
    }, this);
}

void BackgroundManager::setBackgroundOverlayEnabled(const bool enabled) {
    showBackgroundOverlay = enabled;
    createDisplayList();
}

void BackgroundManager::createDisplayList() {
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }

    if (showBackgroundOverlay) {
        for (auto &[r, g, b, a]: colors) {
            r = randomFloat(1.0f, 0.0f);
            g = randomFloat(1.0f, 0.0f);
            b = randomFloat(1.0f, 0.0f);
            a = 1.0f;
        }
    }

    backgroundDisplayList = glGenLists(1);
    glNewList(backgroundDisplayList, GL_COMPILE);
    drawQuad();
    glEndList();
}
