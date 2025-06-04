// BrickManager.cpp
#include "BrickManager.h"

BrickManager::BrickManager(EventManager* evtMgr, TextureManager* texMgr)
    : eventManager(evtMgr), textureManager(texMgr) {}

BrickTexture BrickManager::getTextureForType(BrickType type) {
    switch (type) {
        case BrickType::Blue:      return BrickTexture::Blue;
        case BrickType::Yellow:    return BrickTexture::Yellow;
        case BrickType::Cement:    return BrickTexture::Cement;
        case BrickType::Glass:     return BrickTexture::Glass;
        case BrickType::Green:     return BrickTexture::Green;
        case BrickType::Grey:      return BrickTexture::Grey;
        case BrickType::Purple:    return BrickTexture::Purple;
        case BrickType::White:     return BrickTexture::White;
        case BrickType::Invisible: return BrickTexture::Invisible;
        case BrickType::Red:       return BrickTexture::Red;
        case BrickType::Explosive: return BrickTexture::Explosive;
        case BrickType::Doom:      return BrickTexture::Doom;
        case BrickType::Base:      return BrickTexture::Base;
        default:                   return BrickTexture::Base;
    }
}

void BrickManager::setupBricks(BrickData data) {
    bricks = std::move(data.bricks);
    for (auto& brick : bricks) {
        if (brick.type == BrickType::None) continue;
        brick.texture = textureManager->getBrickTexture(getTextureForType(brick.type));

        switch (brick.type) {
            case BrickType::Glass:
                brick.health = 2.0f;
                break;
            case BrickType::Invisible:
                brick.health = 3.0f;
                brick.visible = false;
                break;
            case BrickType::Cement:
                brick.health = 4.0f;
                break;
            default:
                brick.health = 1.0f;
                break;
        }
        // Für Base-Typ (custom color) die Farbinformationen setzen
        //if (brick.type == BrickType::Base) {
        //    brick.texture->textureProperties.glTexColorInfo[0] = brick.color.r;
        //    brick.texture->textureProperties.glTexColorInfo[1] = brick.color.g;
        //    brick.texture->textureProperties.glTexColorInfo[2] = brick.color.b;
        //    brick.texture->textureProperties.glTexColorInfo[3] = brick.color.a;
        //}
    }

}

void BrickManager::onLevelChanged(const LevelEventData &data) {
    setupBricks(data.brickData);
}

void BrickManager::update(const float deltaTime) {
    // > down 10000
    // The above makes the bricks drop down once every 10 seconds.
    for (auto& brick : bricks) {
        if (brick.isActive()) {
            brick.update(deltaTime);
        }
    }
}

void BrickManager::draw(const float deltaTime) {
    for (auto& Brick : bricks) {
        if (Brick.isActive()) {
            Brick.draw(deltaTime);
        }
    }
}

void BrickManager::onBrickHit(const EventData &data) {
    auto *brick = static_cast<Brick *>(data.target);

    brick->health -= 1.0f;

    switch (brick->type) {
        case BrickType::Invisible:
            if (!brick->visible) {
                brick->visible = true; // Wird sichtbar nach erstem Treffer
            }
            break;

        case BrickType::Explosive:
            if (brick->health <= 0) {
                //detonateExplosiveBrick(brick);
            }
            break;
        default: ;
    }
    if (brick->health <= 0) {
        brick->active = false;

        EventData destroyData;
        destroyData.sender = brick;
        //destroyData.brickId = brick->id;
        // basis Punkte
        destroyData.posX = brick->pos_x;
        destroyData.posY = brick->pos_y;
        eventManager->emit(GameEvent::BrickDestroyed, destroyData);
    }
}
