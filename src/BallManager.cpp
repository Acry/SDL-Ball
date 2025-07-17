#include "BallManager.h"
#include <algorithm>
#include <cmath>
#include <ranges>
#include <SDL2/SDL.h>

BallManager::BallManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr)
    : eventManager(evtMgr), textureManager(texMgr), animationManager(animMgr) {
}

BallManager::~BallManager() {
    clear();
    for (auto *ball: managedObjects) {
        delete ball;
    }
    managedObjects.clear();
}

void BallManager::setExplosive(Ball *ball, bool explosive) {
    if (!ball) return;
    const auto it = std::ranges::find(managedObjects, ball);
    if (it == managedObjects.end()) return;
    const size_t ballIndex = std::distance(managedObjects.begin(), it);
    ball->setExplosive(explosive);

    if (explosive) {
        const texture *fireballTexture = textureManager->getBallTexture(BallTexture::Fireball);
        if (fireballTexture) {
            ball->textureProperties = fireballTexture->textureProperties;
            ball->animProps = fireballTexture->animationProperties;
            if (ball->animProps.frames > 1) {
                animationManager->registerForAnimation(ball, ball->animProps, ball->textureProperties.uvCoordinates);
                animationIndices.push_back(ballIndex);
            }
        }
    } else {
        const texture *normalTexture = textureManager->getBallTexture(BallTexture::Normal);
        if (normalTexture) {
            ball->textureProperties = normalTexture->textureProperties;
            ball->animProps = normalTexture->animationProperties;
            animationManager->unregisterFromAnimation(ball, ball->animProps);
            std::erase(animationIndices, ballIndex);
        }
    }
}

void BallManager::despawnBall(Ball *ball) {
    if (!ball) return;
    ball->setPhysicallyActive(false);
    animationManager->unregisterFromAnimation(ball, ball->animProps);

    if (const auto it = std::ranges::find(managedObjects, ball); it != managedObjects.end()) {
        const size_t ballIndex = std::distance(managedObjects.begin(), it);
        std::erase(animationIndices, ballIndex);
        managedObjects.erase(it);
    }
    delete ball;
}

void BallManager::spawn(float x, float y, bool glued, float speed, float angle) {
    for (auto *ball: managedObjects) {
        if (!ball->isPhysicallyActive()) {
            ball->setPhysicallyActive(true);
            ball->setGlued(glued);
            ball->centerX = x;
            ball->centerY = y;
            ball->pos_x = x - ball->width / 2.0f;
            ball->pos_y = y - ball->height / 2.0f;
            selectedBall = ball;
            launchBall();
            return;
        }
    }
    if (managedObjects.size() < MAX_BALLS) {
        const texture *ballTexture = textureManager->getBallTexture(BallTexture::Normal);
        if (!ballTexture) {
            SDL_Log("Error: Konnte Ball-Textur nicht laden");
            return;
        }
        auto *ball = new Ball(*ballTexture);
        ball->setPhysicallyActive(true);
        ball->setGlued(glued);
        ball->centerX = x;
        ball->centerY = y;
        ball->pos_x = x - ball->width / 2.0f;
        ball->pos_y = y - ball->height / 2.0f;
        if (ball->animProps.frames > 1) {
            animationManager->registerForAnimation(ball, ball->animProps, ball->textureProperties.uvCoordinates);
        }
        managedObjects.push_back(ball);
        selectedBall = ball;
        launchBall();
    }
}

void BallManager::update(float deltaTime) {
    for (auto *ball: managedObjects) {
        if (!ball->isPhysicallyActive() || ball->isGlued()) continue;
        ball->update(deltaTime);
    }
    checkBorderCollision();
    checkBallToBallCollision();
}

void BallManager::draw() const {
    for (const auto *ball: managedObjects) {
        if (ball->isActive()) ball->draw();
        if (ball == selectedBall) {
            glLoadIdentity();
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glLineWidth(0.1f);
            glBegin(GL_LINE_LOOP);
            glVertex3f(ball->pos_x, ball->pos_y, 0.0f);
            glVertex3f(ball->pos_x + ball->getWidth(), ball->pos_y, 0.0f);
            glVertex3f(ball->pos_x + ball->getWidth(), ball->pos_y + ball->getHeight(), 0.0f);
            glVertex3f(ball->pos_x, ball->pos_y + ball->getHeight(), 0.0f);
            glEnd();
            glLineWidth(1.0f);
            glDisable(GL_BLEND);
        }
    }
}

void BallManager::clear() {
    for (auto *ball: managedObjects) {
        animationManager->unregisterFromAnimation(ball, ball->animProps);
        ball->setPhysicallyActive(false);
    }
    animationIndices.clear();
}

float BallManager::getRandomLaunchAngle() {
    // π/2 ± 0.087 rad (ca. ±5 Grad)
    return M_PI / 2 + ((rand() % 40 - 20) * 0.01f);
}

void BallManager::launchBall() const {
    if (!selectedBall) return;
    selectedBall->setGlued(false);
    const float randomAngle = getRandomLaunchAngle();
    selectedBall->xvel = selectedBall->velocity * std::cos(randomAngle);
    selectedBall->yvel = selectedBall->velocity * std::sin(randomAngle);
    selectedBall->setAngle(randomAngle);
}

void BallManager::checkBorderCollision() const {
    for (auto *ball: managedObjects) {
        if (!ball->isPhysicallyActive()) continue;

        if (ball->pos_x + ball->width > 1.0f) {
            ball->pos_x = 1.0f - ball->width;
            ball->xvel *= -1;
        } else if (ball->pos_x < -1.0f) {
            ball->pos_x = -1.0f;
            ball->xvel *= -1;
        }

        if (ball->pos_y + ball->height > 1.0f) {
            ball->pos_y = 1.0f - ball->height;
            ball->yvel *= -1;
        } else if (ball->pos_y < -1.0f) {
            ball->pos_y = -1.0f;
            ball->yvel *= -1;
        }
    }
}

void BallManager::checkBallToBallCollision() const {
    for (size_t i = 0; i < managedObjects.size(); ++i) {
        auto *ball1 = managedObjects[i];
        if (!ball1->isPhysicallyActive()) continue;

        for (size_t j = i + 1; j < managedObjects.size(); ++j) {
            auto *ball2 = managedObjects[j];
            if (!ball2->isPhysicallyActive()) continue;

            const float dx = ball2->centerX - ball1->centerX;
            const float dy = ball2->centerY - ball1->centerY;
            const float distance = std::sqrt(dx * dx + dy * dy);

            const float radius1 = ball1->width / 2.0f;
            const float radius2 = ball2->width / 2.0f;

            if (distance < radius1 + radius2) {
                if (distance < 0.0001f) continue;
                const float nx = dx / distance;
                const float ny = dy / distance;
                const float overlap = (radius1 + radius2) - distance;
                ball1->pos_x -= overlap * nx * 0.5f;
                ball1->pos_y -= overlap * ny * 0.5f;
                ball2->pos_x += overlap * nx * 0.5f;
                ball2->pos_y += overlap * ny * 0.5f;
                ball1->centerX = ball1->pos_x + ball1->width / 2.0f;
                ball1->centerY = ball1->pos_y + ball1->height / 2.0f;
                ball2->centerX = ball2->pos_x + ball2->width / 2.0f;
                ball2->centerY = ball2->pos_y + ball2->height / 2.0f;
                const float mass1 = std::powf(radius1, 3);
                const float mass2 = std::powf(radius2, 3);
                const float rvx = ball2->xvel - ball1->xvel;
                const float rvy = ball2->yvel - ball1->yvel;
                const float velAlongNormal = rvx * nx + rvy * ny;
                if (velAlongNormal > 0) continue;
                constexpr float restitution = 1.0f;
                const float impulse = -(1.0f + restitution) * velAlongNormal / (1.0f / mass1 + 1.0f / mass2);
                const float dvx1 = -impulse * nx / mass1;
                const float dvy1 = -impulse * ny / mass1;
                const float dvx2 = impulse * nx / mass2;
                const float dvy2 = impulse * ny / mass2;
                ball1->xvel += dvx1;
                ball1->yvel += dvy1;
                ball2->xvel += dvx2;
                ball2->yvel += dvy2;
                constexpr float MIN_SPEED = 0.3f;
                float speed1 = std::sqrt(ball1->xvel * ball1->xvel + ball1->yvel * ball1->yvel);
                if (speed1 < MIN_SPEED && speed1 > 0.0001f) {
                    ball1->xvel = ball1->xvel * MIN_SPEED / speed1;
                    ball1->yvel = ball1->yvel * MIN_SPEED / speed1;
                }
                float speed2 = std::sqrt(ball2->xvel * ball2->xvel + ball2->yvel * ball2->yvel);
                if (speed2 < MIN_SPEED && speed2 > 0.0001f) {
                    ball2->xvel = ball2->xvel * MIN_SPEED / speed2;
                    ball2->yvel = ball2->yvel * MIN_SPEED / speed2;
                }
                ball1->collisionPoints = *ball1->getCollisionPoints();
                ball2->collisionPoints = *ball2->getCollisionPoints();
            }
        }
    }
}
