// BrickManager.cpp
#include "BrickManager.h"

#include "config.h"

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

void BrickManager::setupBricks(std::vector<BrickInfo> brickInfos) {
    bricks.clear();
    brickHealth.clear();
    brickTypes.clear();

    for (const auto &info: brickInfos) {
        if (info.type == BrickType::None) continue;

        Brick brick;
        brick.pos_x = info.x;
        brick.pos_y = info.y;
        brick.width = BRICK_WIDTH;
        brick.height = BRICK_HEIGHT;
        brick.texture = textureManager->getBrickTexture(getTextureForType(info.type));

        float health;
        switch (info.type) {
            case BrickType::Glass:
                health = 2.0f;
                break;
            case BrickType::Invisible:
                health = 3.0f;
                brick.visible = false;
                break;
            case BrickType::Cement:
                health = 4.0f;
                break;
            default:
                health = 1.0f;
                break;
        }

        if (info.type == BrickType::Base) {
            brick.color = info.CustomBrickColor;
        }
        const size_t index = bricks.size();
        bricks.push_back(std::move(brick));
        brickHealth[index] = health;
        brickTypes[index] = info.type;
    }
}

void BrickManager::onLevelLoaded(const LevelData &data) {
    setupBricks(data.bricks);
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
    const size_t index = static_cast<size_t>(data.target - &bricks[0]);
    float &health = brickHealth[index];
    Brick &brick = bricks[index];
    const BrickType type = brickTypes[index];
    health -= 1.0f;

    switch (type) {
        case BrickType::Invisible:
            if (!brick.visible) {
                brick.visible = true;
            }
            break;
        case BrickType::Explosive:
            if (health <= 0) {
                // detonateExplosiveBrick(index);
            }
            break;
        default: ;
    }

    if (health <= 0) {
        brick.active = false;
        EventData destroyData;
        destroyData.sender = &brick;
        destroyData.posX = brick.pos_x;
        destroyData.posY = brick.pos_y;
        eventManager->emit(GameEvent::BrickDestroyed, destroyData);
    }
}

