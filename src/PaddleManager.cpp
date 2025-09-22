#include "PaddleManager.h"

PaddleManager::PaddleManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr)
    : eventManager(evtMgr), textureManager(texMgr), animationManager(animMgr) {
    eventManager->addListener(GameEvent::MouseCoordinatesNormalized,
                              [this](const MouseCoordinatesNormalizedEventData &data) {
                                  this->handleMouseCoordinatesNormalized(data);
                              }, this);
    eventManager->addListener(GameEvent::KeyboardPaddleMove,
                              [this](const KeyboardMoveEventData &data) {
                                  this->handleKeyboardMove(data);
                              }, this);
}

PaddleManager::~PaddleManager() {
    eventManager->removeListener(GameEvent::MouseCoordinatesNormalized, this);
    eventManager->removeListener(GameEvent::KeyboardPaddleMove, this);
    clear();
}

Paddle *PaddleManager::getActivePaddle() const {
    return activePaddle;
}

void PaddleManager::setGlueLayer(const bool enabled) const {
    if (activePaddle) {
        activePaddle->setGlueLayer(enabled);
        if (enabled && activePaddle->glueLayerAnimProps.frames > 1) {
            animationManager->registerForAnimation(activePaddle, activePaddle->glueLayerAnimProps,
                                                   activePaddle->glueLayerTextureProperties.uvCoordinates);
        } else if (!enabled && activePaddle->glueLayerAnimProps.frames > 1) {
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->glueLayerAnimProps);
        }
    }
}

void PaddleManager::setGunLayer(const bool enabled) const {
    if (activePaddle) {
        activePaddle->setGunLayer(enabled);
        if (enabled && activePaddle->gunLayerAnimProps.frames > 1) {
            animationManager->registerForAnimation(activePaddle, activePaddle->gunLayerAnimProps,
                                                   activePaddle->gunLayerTextureProperties.uvCoordinates);
        } else if (!enabled && activePaddle->gunLayerAnimProps.frames > 1) {
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->gunLayerAnimProps);
        }
    }
}

void PaddleManager::despawn() {
    if (activePaddle) {
        activePaddle->setPhysicallyActive(false);
        if (activePaddle->animProps.frames > 1) {
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->animProps);
        }
        if (activePaddle->getGlueLayer() && activePaddle->glueLayerAnimProps.frames > 1) {
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->glueLayerAnimProps);
        }
        if (activePaddle->getGunLayer() && activePaddle->gunLayerAnimProps.frames > 1) {
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->gunLayerAnimProps);
        }
        const Paddle *oldPaddle = activePaddle;
        activePaddle = nullptr;
        delete oldPaddle;
    }
}

void PaddleManager::spawn() {
    const texture *paddleBaseTexture = textureManager->getPaddleTexture(PaddleTexture::Base);
    auto *paddle = new Paddle(*paddleBaseTexture);

    if (paddle->animProps.frames > 1) {
        animationManager->registerForAnimation(paddle, paddle->animProps,
                                               paddle->textureProperties.uvCoordinates);
    }

    const texture *paddleGlueTexture = textureManager->getPaddleTexture(PaddleTexture::Glue);
    paddle->glueLayerTextureProperties = paddleGlueTexture->textureProperties;
    paddle->glueLayerAnimProps = paddleGlueTexture->animationProperties;

    const texture *paddleGunTexture = textureManager->getPaddleTexture(PaddleTexture::Gun);
    paddle->gunLayerTextureProperties = paddleGunTexture->textureProperties;
    paddle->gunLayerAnimProps = paddleGunTexture->animationProperties;

    paddle->setPhysicallyActive(true);
    activePaddle = paddle;
}

void PaddleManager::update(float deltaTime) const {
    if (activePaddle) {
        activePaddle->update(deltaTime);
        moveTowardsTarget(deltaTime);
        checkBorderCollision();
    }
}

void PaddleManager::moveTowardsTarget(float deltaTime) const {
    if (!activePaddle || moveTargetX == activePaddle->pos_x) {
        return;
    }
    float direction = (moveTargetX > activePaddle->pos_x) ? 1.0f : -1.0f;
    float movement = direction * paddleSpeed * deltaTime;
    if ((direction > 0 && activePaddle->pos_x + movement > moveTargetX) ||
        (direction < 0 && activePaddle->pos_x + movement < moveTargetX)) {
        activePaddle->pos_x = moveTargetX;
    } else {
        activePaddle->pos_x += movement;
    }
}

void PaddleManager::checkBorderCollision() const {
    if (activePaddle) {
        if (activePaddle->pos_x + activePaddle->width > 1.0f) {
            activePaddle->pos_x = 1.0f - activePaddle->width;
        } else if (activePaddle->pos_x < -1.0f) {
            activePaddle->pos_x = -1.0f;
        }
    }
}

void PaddleManager::checkPaddleToPowerupCollision() const {
    // TODO: Implementierung für Powerup-Kollisionen
}

void PaddleManager::draw() const {
    if (activePaddle) {
        activePaddle->draw();
    }
}

void PaddleManager::clear() {
    if (activePaddle) {
        EventData data;
        data.sender = activePaddle;
        eventManager->emit(GameEvent::PaddleDestroyed, data);
        animationManager->unregisterFromAnimation(activePaddle, activePaddle->animProps);
        animationManager->unregisterFromAnimation(activePaddle, activePaddle->glueLayerAnimProps);
        animationManager->unregisterFromAnimation(activePaddle, activePaddle->gunLayerAnimProps);
        activePaddle->setPhysicallyActive(false);
        delete activePaddle;
        activePaddle = nullptr;
    }
}

void PaddleManager::handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) {
    if (activePaddle) {
        paddleSpeed = 10.0f;
        moveTargetX = data.x - activePaddle->getWidth() / 2.0f;
    }
}

void PaddleManager::handleKeyboardMove(const KeyboardMoveEventData &data) {
    if (activePaddle) {
        if (data.direction != 0.0f) {
            moveTargetX = data.direction < 0 ? -1.0f : 1.0f;
        } else {
            moveTargetX = activePaddle->pos_x;
        }
        paddleSpeed = 1.0f;
    }
}
