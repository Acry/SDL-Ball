#include "GrowableObject.h"

void GrowableObject::setGrowTarget(const GLfloat targetWidth) {
    destWidth = targetWidth;
    if (targetWidth > getWidth()) {
        growing = true;
        shrinking = false;
    } else if (targetWidth < getWidth()) {
        growing = false;
        shrinking = true;
    } else {
        growing = false;
        shrinking = false;
    }
}

void GrowableObject::updateGrowth(const float deltaTime) {
    if (!growing && !shrinking) return;
    const GLfloat oldWidth = width;
    const GLfloat oldHeight = height;
    if (growing) {
        const GLfloat growStep = growSpeed * deltaTime;
        setWidth(width + growStep);

        if (keepAspectRatio) {
            setHeight(width / aspectRatio);
        }

        if (width >= destWidth) {
            growing = false;
        }
    } else {
        const GLfloat shrinkStep = growSpeed * deltaTime;
        setWidth(width - shrinkStep);

        if (keepAspectRatio) {
            setHeight(width / aspectRatio);
        }

        if (width <= destWidth) {
            shrinking = false;
        }
    }
    // Berechne die Positionskorrektur, um das Zentrum beizubehalten
    const GLfloat widthDiff = width - oldWidth;
    const GLfloat heightDiff = height - oldHeight;

    // Verschiebe Position nach links/oben, um Wachstum aus dem Zentrum zu ermöglichen
    pos_x -= widthDiff / 2.0f;
    pos_y -= heightDiff / 2.0f;
    onSizeChanged();
}

void GrowableObject::grow(const GLfloat targetWidth) {
    setGrowTarget(targetWidth);
}
