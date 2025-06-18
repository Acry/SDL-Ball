#pragma once
#include <epoxy/gl.h>

#include "MovingObject.h"

class GrowableObject : public MovingObject {
protected:
    bool growing{false};
    bool shrinking{false};
    GLfloat destWidth{0.0f};
    GLfloat growSpeed{0.05f};
    bool keepAspectRatio{true};
    GLfloat aspectRatio{1.0f};

    // Virtueller Hook für Subklassen
    virtual void onSizeChanged() {
    }

public:
    explicit GrowableObject(const texture &tex) : MovingObject(tex) {
    }

    ~GrowableObject() override = default;

    void update(const float deltaTime) override {
        updateGrowth(deltaTime);
        MovingObject::update(deltaTime);
    }

    void init() override = 0;

    void draw() const override = 0;

    [[nodiscard]] virtual GLfloat getWidth() const { return width; }
    [[nodiscard]] virtual GLfloat getHeight() const { return height; }

    virtual void setWidth(const GLfloat w) {
        width = w;
    }

    virtual void setHeight(const GLfloat h) {
        height = h;
    }

    void setGrowTarget(GLfloat targetWidth);

    void updateGrowth(float deltaTime);

    void grow(GLfloat targetWidth);

    // Getter für Status
    [[nodiscard]] bool isGrowing() const { return growing; }
    [[nodiscard]] bool isShrinking() const { return shrinking; }
};
