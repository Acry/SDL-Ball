#pragma once
#include <epoxy/gl.h>

class GrowableObject {
protected:
    bool growing{false};
    bool shrinking{false};
    GLfloat destWidth{0.0f};
    GLfloat growSpeed{0.1f};
    bool keepAspectRatio{true};
    GLfloat aspectRatio{1.0f};

    // Virtueller Hook für Subklassen
    virtual void onSizeChanged() {}

public:
    virtual ~GrowableObject() = default;

    // Virtuelle Getter und Setter für Größe (zu implementieren in Subklassen)
    virtual GLfloat getWidth() const = 0;
    virtual GLfloat getHeight() const = 0;
    virtual void setWidth(GLfloat w) = 0;
    virtual void setHeight(GLfloat h) = 0;

    void setGrowTarget(GLfloat targetWidth);
    void updateGrowth(float deltaTime);

    // Getter für Status
    bool isGrowing() const { return growing; }
    bool isShrinking() const { return shrinking; }
};
