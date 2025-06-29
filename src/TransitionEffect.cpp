// TransitionEffect.cpp
#include "TransitionEffect.h"
#include <algorithm>

TransitionEffect::TransitionEffect() {
    properties.type = TransitionEffectType::FadeIn;
    properties.life = 1000;
    properties.color = {0.0f, 0.0f, 0.0f, 1.0f};
    properties.category = EffectCategory::Transition;
    properties.active = true; // Standardmäßig aktiv setzen
    properties.startOpacity = 1.0f;
    properties.endOpacity = 0.0f;
    opacity = properties.startOpacity;
    age = 0;
}

void TransitionEffect::init(const position &pos) {
}

void TransitionEffect::update(float deltaTime) {
    age += static_cast<int>(deltaTime * 1000.0f);
    float progress = std::min(1.0f, static_cast<float>(age) / static_cast<float>(properties.life));

    switch (properties.type) {
        case TransitionEffectType::FadeIn:
        case TransitionEffectType::FadeOut:
        case TransitionEffectType::CrossFade:
            // Opazität für Fade-Effekte aktualisieren
            opacity = properties.startOpacity + (properties.endOpacity - properties.startOpacity) * progress;
            break;

        case TransitionEffectType::SlideIn:
            // Position von außerhalb nach innen bewegen
            properties.rect.x = -2.0f + 3.0f * progress; // Von -2 nach 1
            break;

        case TransitionEffectType::SlideOut:
            // Position von innen nach außerhalb bewegen
            properties.rect.x = 3.0f * progress - 1.0f; // Von -1 nach 2
            break;

        case TransitionEffectType::WipeIn:
            // Wischkante von links nach rechts bewegen
            properties.rect.x = -1.0f + 2.0f * progress; // Von -1 nach 1
            break;

        case TransitionEffectType::WipeOut:
            // Wischkante von rechts nach links bewegen
            properties.rect.x = 1.0f - 2.0f * progress; // Von 1 nach -1
            break;
    }

    if (age > properties.life) {
        properties.active = false;
    }
}

void TransitionEffect::draw() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();

    switch (properties.type) {
        case TransitionEffectType::FadeIn:
        case TransitionEffectType::FadeOut:
        case TransitionEffectType::CrossFade:
            glDisable(GL_TEXTURE_2D);
            glColor4f(properties.color.r, properties.color.g, properties.color.b, opacity);
            glBegin(GL_QUADS);
            glVertex3f(-1.0f, 1.0f, 0.0);
            glVertex3f(1.0f, 1.0f, 0.0);
            glVertex3f(1.0f, -1.0f, 0.0);
            glVertex3f(-1.0f, -1.0f, 0.0);
            glEnd();
            break;

        case TransitionEffectType::SlideIn:
            if (currentTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, currentTextureId);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(properties.rect.x, -1.0f, 0.0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(properties.rect.x + 2.0f, -1.0f, 0.0);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(properties.rect.x + 2.0f, 1.0f, 0.0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(properties.rect.x, 1.0f, 0.0);
                glEnd();
            }
            break;

        case TransitionEffectType::SlideOut:
            if (currentTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, currentTextureId);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(properties.rect.x, -1.0f, 0.0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(properties.rect.x + 2.0f, -1.0f, 0.0);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(properties.rect.x + 2.0f, 1.0f, 0.0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(properties.rect.x, 1.0f, 0.0);
                glEnd();
            }
            break;

        case TransitionEffectType::WipeIn:
            if (currentTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, currentTextureId);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                // Texturkoordinaten anpassen, um nur den sichtbaren Teil zu zeigen
                float texCoordX = (properties.rect.x + 1.0f) / 2.0f; // Von -1,1 auf 0,1 normalisieren
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, 0.0);
                glTexCoord2f(texCoordX, 0.0f);
                glVertex3f(properties.rect.x, -1.0f, 0.0);
                glTexCoord2f(texCoordX, 1.0f);
                glVertex3f(properties.rect.x, 1.0f, 0.0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-1.0f, 1.0f, 0.0);
                glEnd();
            }
            break;

        case TransitionEffectType::WipeOut:
            if (currentTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, currentTextureId);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                // Texturkoordinaten anpassen, um nur den sichtbaren Teil zu zeigen
                float texCoordX = (1.0f - properties.rect.x) / 2.0f; // Von -1,1 auf 0,1 normalisieren
                glTexCoord2f(texCoordX, 0.0f);
                glVertex3f(properties.rect.x, -1.0f, 0.0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(1.0f, -1.0f, 0.0);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(1.0f, 1.0f, 0.0);
                glTexCoord2f(texCoordX, 1.0f);
                glVertex3f(properties.rect.x, 1.0f, 0.0);
                glEnd();
            }
            break;
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

bool TransitionEffect::isActive() const {
    return properties.active;
}
