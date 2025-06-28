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
    glDisable(GL_TEXTURE_2D);

    switch (properties.type) {
        case TransitionEffectType::FadeIn:
        case TransitionEffectType::FadeOut:
        case TransitionEffectType::CrossFade:
            // Vollbild-Fade-Effekt
            glColor4f(properties.color.r, properties.color.g, properties.color.b, opacity);
            glBegin(GL_QUADS);
            glVertex3f(-1.0f, 1.0f, 0.0);
            glVertex3f(1.0f, 1.0f, 0.0);
            glVertex3f(1.0f, -1.0f, 0.0);
            glVertex3f(-1.0f, -1.0f, 0.0);
            glEnd();
            break;

        case TransitionEffectType::SlideIn:
        case TransitionEffectType::SlideOut:
            // Gleitendes Rechteck
            glColor4f(properties.color.r, properties.color.g, properties.color.b, 1.0f);
            glBegin(GL_QUADS);
            glVertex3f(properties.rect.x, 1.0f, 0.0);
            glVertex3f(properties.rect.x + 2.0f, 1.0f, 0.0); // 2.0 Einheiten breit
            glVertex3f(properties.rect.x + 2.0f, -1.0f, 0.0);
            glVertex3f(properties.rect.x, -1.0f, 0.0);
            glEnd();
            break;

        case TransitionEffectType::WipeIn:
        case TransitionEffectType::WipeOut:
            // Wisch-Effekt mit bewegender Kante
            glColor4f(properties.color.r, properties.color.g, properties.color.b, 1.0f);
            glBegin(GL_QUADS);
            glVertex3f(-1.0f, 1.0f, 0.0);
            glVertex3f(properties.rect.x, 1.0f, 0.0);
            glVertex3f(properties.rect.x, -1.0f, 0.0);
            glVertex3f(-1.0f, -1.0f, 0.0);
            glEnd();
            break;
    }

    glDisable(GL_BLEND);
}

bool TransitionEffect::isActive() const {
    return properties.active;
}
