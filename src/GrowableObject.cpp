#include "GrowableObject.h"

void GrowableObject::setGrowTarget(GLfloat targetWidth) {
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

void GrowableObject::updateGrowth(float deltaTime) {
    if (growing) {
        GLfloat newsize = growSpeed * deltaTime;
        setWidth(getWidth() + newsize);

        if (getWidth() >= destWidth) {
            setWidth(destWidth);
            growing = false;
        }

        // Höhe anpassen
        if (keepAspectRatio) {
            setHeight(getWidth() * aspectRatio);
        }

        // Subklassen benachrichtigen
        onSizeChanged();
    } else if (shrinking) {
        GLfloat newsize = growSpeed * deltaTime;
        setWidth(getWidth() - newsize);

        if (getWidth() <= destWidth) {
            setWidth(destWidth);
            shrinking = false;
        }

        // Höhe anpassen
        if (keepAspectRatio) {
            setHeight(getWidth() * aspectRatio);
        }

        // Subklassen benachrichtigen
        onSizeChanged();
    }
}
