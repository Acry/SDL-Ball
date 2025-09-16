// GrowableObject_aTests.cpp
#include <gtest/gtest.h>
#include "GrowableObject.h"
#include "MovingObject.h"

class TestGrowableObject final : public GrowableObject {
public:
    TestGrowableObject(const GLfloat width, const GLfloat height, const GLfloat aspectRatio)
        : GrowableObject(texture{}) {
        this->width = width;
        this->height = height;
        this->aspectRatio = aspectRatio;
        this->keepAspectRatio = true;
    }

    // Implementierung der abstrakten Methoden
    void init() override {
    }

    void draw() const override {
    }

    void publicUpdateGrowth(const float deltaTime) {
        updateGrowth(deltaTime);
    }

    void publicSetGrowTarget(const GLfloat target) {
        setGrowTarget(target);
    }
};

TEST(GrowableObjectTest, KeepsAspectRatioWhenGrowing) {
    // Initialisierung mit normalem Aspektverhältnis (Breite/Höhe)
    constexpr GLfloat initialWidth = 0.2f;
    constexpr GLfloat initialHeight = 0.05f;
    constexpr GLfloat aspectRatio = initialWidth / initialHeight; // ca. 4.0

    TestGrowableObject obj(initialWidth, initialHeight, aspectRatio);

    // Wachstumsziel setzen (20% größer)
    constexpr GLfloat targetWidth = initialWidth * 1.2f;
    obj.publicSetGrowTarget(targetWidth);

    // Überprüfe, dass das Objekt wächst
    EXPECT_TRUE(obj.isGrowing());
    EXPECT_FALSE(obj.isShrinking());

    // Wachstum simulieren (großer deltaTime für sofortige Veränderung)
    obj.publicUpdateGrowth(10.0f);

    // Überprüfe, ob die Breite korrekt angepasst wurde
    EXPECT_FLOAT_EQ(obj.getWidth(), targetWidth);

    // Überprüfe, ob das Aspektverhältnis korrekt beibehalten wurde
    const GLfloat newAspectRatio = obj.getWidth() / obj.getHeight();
    EXPECT_NEAR(newAspectRatio, aspectRatio, 0.00001f);

    // Überprüfe, ob die Höhe korrekt berechnet wurde
    constexpr GLfloat expectedHeight = targetWidth / aspectRatio;
    EXPECT_FLOAT_EQ(obj.getHeight(), expectedHeight);
}

TEST(GrowableObjectTest, KeepsAspectRatioWhenShrinking) {
    constexpr GLfloat initialWidth = 0.2f;
    constexpr GLfloat initialHeight = 0.05f;
    constexpr GLfloat aspectRatio = initialWidth / initialHeight;

    TestGrowableObject obj(initialWidth, initialHeight, aspectRatio);

    // Schrumpfungsziel setzen (20% kleiner)
    constexpr GLfloat targetWidth = initialWidth * 0.8f;
    obj.publicSetGrowTarget(targetWidth);

    // Überprüfe, dass das Objekt schrumpft
    EXPECT_FALSE(obj.isGrowing());
    EXPECT_TRUE(obj.isShrinking());

    // Schrumpfung simulieren
    obj.publicUpdateGrowth(10.0f);

    // Überprüfe, ob die Breite korrekt angepasst wurde
    EXPECT_FLOAT_EQ(obj.getWidth(), targetWidth);

    // Überprüfe, ob das Aspektverhältnis korrekt beibehalten wurde
    const GLfloat newAspectRatio = obj.getWidth() / obj.getHeight();
    EXPECT_NEAR(newAspectRatio, aspectRatio, 0.00001f);
}

TEST(GrowableObjectTest, GrowingStopsAtTargetWidth) {
    TestGrowableObject obj(0.2f, 0.05f, 4.0f);

    constexpr GLfloat targetWidth = 0.24f;
    obj.publicSetGrowTarget(targetWidth);

    // Kleine Änderung, sollte nicht das Ziel erreichen
    obj.publicUpdateGrowth(0.1f);
    EXPECT_LT(obj.getWidth(), targetWidth);
    EXPECT_TRUE(obj.isGrowing());

    // Große Änderung, sollte das Ziel erreichen und stoppen
    obj.publicUpdateGrowth(10.0f);
    EXPECT_FLOAT_EQ(obj.getWidth(), targetWidth);
    EXPECT_FALSE(obj.isGrowing());
}

TEST(GrowableObjectTest, InvertedAspectRatioGrowsIncorrectly) {
    // Hier testen wir das Problem mit dem umgekehrten Aspektverhältnis
    constexpr GLfloat initialWidth = 0.2f;
    constexpr GLfloat initialHeight = 0.05f;

    // Aspektverhältnis falsch herum definiert (Höhe/Breite statt Breite/Höhe)
    constexpr GLfloat invertedAspectRatio = initialHeight / initialWidth; // ca. 0.25

    TestGrowableObject obj(initialWidth, initialHeight, invertedAspectRatio);

    // Wachstumsziel setzen
    constexpr GLfloat targetWidth = initialWidth * 1.2f;
    obj.publicSetGrowTarget(targetWidth);

    // Wachstum simulieren
    obj.publicUpdateGrowth(10.0f);

    // Bei falsch definiertem Aspektverhältnis würde die Höhe falsch berechnet:
    // width / invertedAspectRatio = width / (height/width) = width * width / height
    // Das führt dazu, dass die Höhe quadratisch zur Breite wächst!

    // Die Höhe sollte NICHT der erwarteten Höhe entsprechen, wenn das Aspektverhältnis korrekt wäre
    constexpr GLfloat correctExpectedHeight = targetWidth / (initialWidth / initialHeight);
    EXPECT_NE(obj.getHeight(), correctExpectedHeight);

    // Stattdessen sollte die Höhe dem falschen Berechnungsergebnis entsprechen
    constexpr GLfloat incorrectHeight = targetWidth / invertedAspectRatio;
    EXPECT_FLOAT_EQ(obj.getHeight(), incorrectHeight);
}

TEST(GrowableObjectTest, CenterPositionMaintainedWhenGrowing) {
    constexpr GLfloat initialWidth = 0.2f;
    constexpr GLfloat initialHeight = 0.05f;
    constexpr GLfloat aspectRatio = initialWidth / initialHeight;

    TestGrowableObject obj(initialWidth, initialHeight, aspectRatio);

    // Setze eine Position
    obj.pos_x = 0.4f;
    obj.pos_y = 0.3f;

    // Berechne Zentrumsposition vor dem Wachstum
    const GLfloat initialCenterX = obj.pos_x + initialWidth / 2.0f;
    const GLfloat initialCenterY = obj.pos_y + initialHeight / 2.0f;

    // Wachstumsziel setzen
    obj.publicSetGrowTarget(initialWidth * 1.5f);

    // Wachstum simulieren
    obj.publicUpdateGrowth(10.0f);

    // Berechne Zentrumsposition nach dem Wachstum
    const GLfloat newCenterX = obj.pos_x + obj.getWidth() / 2.0f;
    const GLfloat newCenterY = obj.pos_y + obj.getHeight() / 2.0f;

    // Überprüfe, ob das Zentrum beibehalten wurde
    EXPECT_NEAR(initialCenterX, newCenterX, 0.00001f);
    EXPECT_NEAR(initialCenterY, newCenterY, 0.00001f);
}
