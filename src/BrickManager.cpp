// BrickManager.cpp
#include "BrickManager.h"

#include "config.h"

BrickManager::BrickManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr)
    : eventManager(evtMgr), textureManager(texMgr), spriteSheetAnimationManager(animMgr) {
    eventManager->addListener(GameEvent::BallHitBrick,
                              [this](const CollisionData &data) { onBallHitBrick(data); },
                              this);

    eventManager->addListener(GameEvent::LevelLoaded,
                              [this](const LevelData &data) { onLevelLoaded(data); },
                              this);
}

BrickTexture BrickManager::getTextureForType(const BrickType type) {
    switch (type) {
        case BrickType::Blue: return BrickTexture::Blue;
        case BrickType::Yellow: return BrickTexture::Yellow;
        case BrickType::Cement: return BrickTexture::Cement;
        case BrickType::Glass: return BrickTexture::Glass;
        case BrickType::Green: return BrickTexture::Green;
        case BrickType::Grey: return BrickTexture::Grey;
        case BrickType::Purple: return BrickTexture::Purple;
        case BrickType::White: return BrickTexture::White;
        case BrickType::Invisible: return BrickTexture::Invisible;
        case BrickType::Red: return BrickTexture::Red;
        case BrickType::Explosive: return BrickTexture::Explosive;
        case BrickType::Doom: return BrickTexture::Doom;
        default: return BrickTexture::Base;
    }
}

void BrickManager::setupBricks(const std::vector<BrickInfo> &brickInfos) {
    clear();

    bricks.reserve(brickInfos.size());
    brickHealth.reserve(brickInfos.size());
    brickTypes.reserve(brickInfos.size());

    for (const auto &[type, x, y, CustomBrickColor]: brickInfos) {
        if (type == BrickType::None) continue;

        Brick brick(*textureManager->getBrickTexture(getTextureForType(type)));
        brick.pos_x = x;
        brick.pos_y = y;
        brick.width = BRICK_WIDTH;
        brick.height = BRICK_HEIGHT;

        float health;
        switch (type) {
            case BrickType::Glass:
                health = 2.0f;
                break;
            case BrickType::Invisible:
                health = 3.0f;
                brick.setVisible(false);
                break;
            case BrickType::Cement:
                health = 4.0f;
                break;
            default:
                health = 1.0f;
                break;
        }

        if (type == BrickType::Base) {
            brick.customColor = CustomBrickColor;
        }
        const bool hasAnimation = brick.animProps.frames > 1;

        const size_t index = bricks.size();
        brickHealth[index] = health;
        brickTypes[index] = type;
        bricks.push_back(std::move(brick));

        if (hasAnimation) {
            animationIndices.push_back(index);
        }
    }
    for (const size_t idx: animationIndices) {
        if (idx < bricks.size() && bricks[idx].animProps.frames > 1) {
            spriteSheetAnimationManager->registerForAnimation(&bricks[idx], bricks[idx].animProps);
        }
    }
}

void BrickManager::onLevelLoaded(const LevelData &data) {
    setupBricks(data.bricks);
}

void BrickManager::update(const float deltaTime) {
    // > down 10000
    // The above makes the bricks drop down once every 10 seconds.
    for (auto &brick: bricks) {
        if (brick.isActive()) {
            brick.update(deltaTime);
        }
    }
}

void BrickManager::draw() const {
    for (auto &brick: bricks) {
        if (brick.isActive() && brick.isVisible()) {
            brick.draw();
        }
    }
}

void BrickManager::onBallHitBrick(const CollisionData &data) {
    const auto *hitBrick = dynamic_cast<const Brick *>(data.object2);
    const auto index = static_cast<size_t>(hitBrick - &bricks[0]);

    float &health = brickHealth[index];
    auto &brick = bricks[index];
    const BrickType type = brickTypes[index];
    health -= 1.0f;

    switch (type) {
        case BrickType::Invisible:
            if (!brick.isVisible()) {
                brick.setVisible(true);
            }
            break;
        case BrickType::Explosive:
            if (health <= 0) {
                // detonateExplosiveBrick(index);
                // Emit explosion event x , y
            }
            break;
        default: ;
    }

    if (health <= 0) {
        brick.setActive(false);
        spriteSheetAnimationManager->unregisterFromAnimation(&brick);

        // Aus der Liste entfernen und löschen
        EventData destroyData;
        destroyData.sender = &brick;
        destroyData.posX = brick.pos_x;
        destroyData.posY = brick.pos_y;
        eventManager->emit(GameEvent::BrickDestroyed, destroyData);
    }
}

void BrickManager::clear() {
    for (const size_t idx: animationIndices) {
        if (idx < bricks.size()) {
            spriteSheetAnimationManager->unregisterFromAnimation(&bricks[idx]);
        }
    }
    bricks.clear();
    brickHealth.clear();
    brickTypes.clear();
    animationIndices.clear();
}

size_t BrickManager::getActiveBrickCount() const {
    return std::count_if(bricks.begin(), bricks.end(),
                         [](const Brick &brick) { return brick.isActive(); });
}

BrickManager::~BrickManager() {
    if (eventManager) {
        eventManager->removeListener(GameEvent::BallHitBrick, this);
        eventManager->removeListener(GameEvent::LevelLoaded, this);
    }
}
