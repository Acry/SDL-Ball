#include <gtest/gtest.h>
#include "../../src/CollisionManager.h"
#include <vector>

// Mock-Klasse für ICollideable
class MockCollideable : public ICollideable {
public:
    MockCollideable(float posX, float posY, float width, float height, bool active = true,
                   int collisionType = 0)
        : posX(posX), posY(posY), width(width), height(height),
          active(active), collisionType(collisionType), collisionCount(0) {}

    float getPosX() const override { return posX; }
    float getPosY() const override { return posY; }
    float getWidth() const override { return width; }
    float getHeight() const override { return height; }
    bool isActive() const override { return active; }
    int getCollisionType() const override { return collisionType; }

    void onCollision(ICollideable* other, float hitX, float hitY) override {
        collisionCount++;
        lastHitX = hitX;
        lastHitY = hitY;
        lastCollidesWith = other;
    }

    // Test-Hilfsvariablen
    float posX, posY, width, height;
    bool active;
    int collisionType;
    int collisionCount = 0;
    float lastHitX = 0.0f, lastHitY = 0.0f;
    ICollideable* lastCollidesWith = nullptr;
};

class CollisionManagerTest : public ::testing::Test {
protected:
    // Einfache nicht-überlappende Objekte (NDC-Koordinaten)
    MockCollideable obj1{-0.8f, -0.8f, 0.05f, 0.05f};
    MockCollideable obj2{0.8f, 0.8f, 0.05f, 0.05f};

    // Objekte mit Überlappung
    MockCollideable obj3{-0.75f, -0.75f, 0.05f, 0.05f};

    // Randobjekte in NDC
    MockCollideable leftBorder{-1.0f, 1.0f, 0.05f, 2.0f, true,
                             static_cast<int>(CollisionType::BorderLeft)};
    MockCollideable rightBorder{1.0f, 1.0f, 0.05f, 2.0f, true,
                              static_cast<int>(CollisionType::BorderRight)};
    MockCollideable topBorder{0.0f, 1.0f, 1.9f, 0.05f, true,
                            static_cast<int>(CollisionType::BorderTop)};

    MockCollideable obj4{-1.0f, -0.95f, 2.0f, 0.05f};
};

TEST_F(CollisionManagerTest, NoCollisionWhenObjectsApartAABB) {
    EXPECT_FALSE(CollisionManager::checkCollision(obj1, obj2));
}

TEST_F(CollisionManagerTest, DetectsCollisionWhenObjectsOverlap) {
    EXPECT_TRUE(CollisionManager::checkCollision(obj1, obj3));
}

TEST_F(CollisionManagerTest, CollisionPointCalculation) {
    float hitX = 0.0f, hitY = 0.0f;

    EXPECT_TRUE(CollisionManager::checkCollision(obj1, obj3, hitX, hitY));

    // Die Kollisionspunktberechnung erfordert spezifische Erwartungen
    // basierend auf der Implementierung
    EXPECT_TRUE(hitX == obj1.posX + obj1.width || hitX == obj1.posX - obj1.width ||
               hitY == obj1.posY + obj1.height || hitY == obj1.posY - obj1.height);
}

TEST_F(CollisionManagerTest, ProcessCollisionsDetectsAndNotifies) {
    std::vector<ICollideable*> objects = { &obj1, &obj3 };

    CollisionManager::processCollisions(objects);

    EXPECT_EQ(1, obj1.collisionCount);
    EXPECT_EQ(1, obj3.collisionCount);
    EXPECT_EQ(&obj3, obj1.lastCollidesWith);
    EXPECT_EQ(&obj1, obj3.lastCollidesWith);
}

TEST_F(CollisionManagerTest, InactiveObjectsIgnored) {
    obj3.active = false;
    std::vector<ICollideable*> objects = { &obj1, &obj3 };

    CollisionManager::processCollisions(objects);

    EXPECT_EQ(0, obj1.collisionCount);
    EXPECT_EQ(0, obj3.collisionCount);
}

TEST_F(CollisionManagerTest, CollisionWithLeftBorder) {
    // Platziere Ball nahe am linken Rand
    MockCollideable ball{-0.95f, 0.0f, 0.05f, 0.05f};

    float hitX = 0.0f, hitY = 0.0f;
    bool collision = CollisionManager::checkCollisionWithBorder(ball, leftBorder, hitX, hitY);

    EXPECT_TRUE(collision);
    EXPECT_FLOAT_EQ(leftBorder.posX + leftBorder.width, hitX);
    EXPECT_FLOAT_EQ(ball.posY, hitY);
}

TEST_F(CollisionManagerTest, CollisionWithRightBorder) {
    // Platziere Ball nahe am rechten Rand
    MockCollideable ball{0.95f, 0.0f, 0.05f, 0.05f};

    float hitX = 0.0f, hitY = 0.0f;
    bool collision = CollisionManager::checkCollisionWithBorder(ball, rightBorder, hitX, hitY);

    EXPECT_TRUE(collision);
    EXPECT_FLOAT_EQ(rightBorder.posX, hitX);
    EXPECT_FLOAT_EQ(ball.posY, hitY);
}

TEST_F(CollisionManagerTest, CollisionWithTopBorder) {
    // Platziere Ball nahe am oberen Rand
    MockCollideable ball{0.0f, 0.95f, 0.05f, 0.05f};

    float hitX = 0.0f, hitY = 0.0f;
    bool collision = CollisionManager::checkCollisionWithBorder(ball, topBorder, hitX, hitY);

    EXPECT_TRUE(collision);
    EXPECT_FLOAT_EQ(ball.posX, hitX);
    EXPECT_FLOAT_EQ(topBorder.posY-topBorder.height, hitY);
}

TEST_F(CollisionManagerTest, checkCollisionTop) {
    MockCollideable ball{0.0f, 0.95f, 0.05f, 0.05f};

    bool collision = CollisionManager::checkCollision(ball, topBorder);

    EXPECT_TRUE(collision);
}

TEST_F(CollisionManagerTest, checkCollisionLeft) {
    MockCollideable ball{-0.95f, 0.0f, 0.05f, 0.05f};
    bool collision = CollisionManager::checkCollision(ball, leftBorder);

    EXPECT_TRUE(collision);
}

TEST_F(CollisionManagerTest, checkCollisionRight) {
    MockCollideable ball{0.95f, 0.0f, 0.05f, 0.05f};

    bool collision = CollisionManager::checkCollision(ball, rightBorder);

    EXPECT_TRUE(collision);
}

TEST_F(CollisionManagerTest, checkCollisionBottom) {
    MockCollideable ball{0.0f, -0.95f, 0.05f, 0.05f};

    bool collision = CollisionManager::checkCollision(ball, obj4);

    EXPECT_TRUE(collision);

}

TEST_F(CollisionManagerTest, NoCollisionWithBorder) {
    // Platziere Ball weit weg vom Rand
    MockCollideable ball{0.0f, 0.0f, 0.05f, 0.05f};

    float hitX = 0.0f, hitY = 0.0f;
    bool collision = CollisionManager::checkCollisionWithBorder(ball, topBorder, hitX, hitY);

    EXPECT_FALSE(collision);
}
