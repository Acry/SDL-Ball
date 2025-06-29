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
            // Zuerst aktuelle Textur rendern
            if (currentTextureId > 0) {
                renderTexture(currentTextureId, 1.0f);
            }
            // Dann nächste Textur mit Opacity
            if (nextTextureId > 0) {
                renderTexture(nextTextureId, opacity);
            }
            break;

        case TransitionEffectType::FadeOut:
            // Nur aktuelle Textur mit abnehmender Opacity
            if (currentTextureId > 0) {
                renderTexture(currentTextureId, 1.0f - opacity);
            }
            // Hintergrund schwarz oder nächste Textur
            if (nextTextureId > 0) {
                renderTexture(nextTextureId, 1.0f);
            }
            break;

        case TransitionEffectType::CrossFade:
            // Beide Texturen mit entgegengesetzter Opacity
            if (currentTextureId > 0) {
                renderTexture(currentTextureId, 1.0f - opacity);
            }
            if (nextTextureId > 0) {
                renderTexture(nextTextureId, opacity);
            }
            break;

        case TransitionEffectType::SlideIn:
            // Aktuelle Textur statisch
            if (currentTextureId > 0) {
                renderTexture(currentTextureId, 1.0f);
            }
            // Nächste Textur schiebt sich rein
            if (nextTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, nextTextureId);
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
                glDisable(GL_TEXTURE_2D);
            }
            break;

        case TransitionEffectType::SlideOut:
            // Nächste Textur statisch im Hintergrund
            if (nextTextureId > 0) {
                renderTexture(nextTextureId, 1.0f);
            }
            // Aktuelle Textur schiebt sich raus
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
                glDisable(GL_TEXTURE_2D);
            }
            break;

        case TransitionEffectType::WipeIn:
            // Aktuelle Textur im Hintergrund
            if (currentTextureId > 0) {
                renderTexture(currentTextureId, 1.0f);
            }
            // Nächste Textur wird aufgedeckt
            if (nextTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, nextTextureId);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, 0.0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(properties.rect.x, -1.0f, 0.0);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(properties.rect.x, 1.0f, 0.0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-1.0f, 1.0f, 0.0);
                glEnd();
                glDisable(GL_TEXTURE_2D);
            }
            break;

        case TransitionEffectType::WipeOut:
            // Nächste Textur im Hintergrund
            if (nextTextureId > 0) {
                renderTexture(nextTextureId, 1.0f);
            }
            // Aktuelle Textur wird verdeckt
            if (currentTextureId > 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, currentTextureId);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(properties.rect.x, -1.0f, 0.0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(1.0f, -1.0f, 0.0);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(1.0f, 1.0f, 0.0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(properties.rect.x, 1.0f, 0.0);
                glEnd();
                glDisable(GL_TEXTURE_2D);
            }
            break;
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

bool TransitionEffect::isActive() const {
    return properties.active;
}

// Hilfsmethode zum Rendern einer Textur mit Opazität
void TransitionEffect::renderTexture(GLuint textureId, float alpha) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
