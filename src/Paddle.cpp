#include "Paddle.h"
#include <epoxy/gl.h>

Paddle::Paddle(EventManager *eventMgr) : MovingObject() {
    // eventManager direkt zuweisen entfällt, da es jetzt durch GameObject verwaltet wird
    this->eventManager = eventMgr; // Zugriff auf das geschützte Mitglied der Basisklasse
    init();
}

void Paddle::init() {
    // GrowableObject-Eigenschaften
    growing = false;
    shrinking = false;
    growSpeed = 0.05f;
    aspectRatio = 0.2f;
    keepAspectRatio = true;

    // xvel = 0.0f;?
    // GameObject-Eigenschaften (und GrowableObject)
    pos_y = -0.955f;
    pos_x = 0.0f;
    width = 0.062f;
    height = 0.016f;

    // Paddle-spezifische Eigenschaften
    dead = false;
    hasGlueLayer = false;
    hasGunLayer = false;
}

void Paddle::drawBase() const {
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
    glColor4f(texture.textureProperties.glTexColorInfo[0],
              texture.textureProperties.glTexColorInfo[1],
              texture.textureProperties.glTexColorInfo[2],
              texture.textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(texture.texturePosition[0], texture.texturePosition[1]);
    glVertex3f(-width, height, 0.0f);
    glTexCoord2f(texture.texturePosition[2], texture.texturePosition[3]);
    glVertex3f(width, height, 0.0f);
    glTexCoord2f(texture.texturePosition[4], texture.texturePosition[5]);
    glVertex3f(width, -height, 0.0f);
    glTexCoord2f(texture.texturePosition[6], texture.texturePosition[7]);
    glVertex3f(-width, -height, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Paddle::drawGlueLayer() const {
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, layerTex[0].textureProperties.texture);
    glColor4f(layerTex[0].textureProperties.glTexColorInfo[0],
              layerTex[0].textureProperties.glTexColorInfo[1],
              layerTex[0].textureProperties.glTexColorInfo[2],
              layerTex[0].textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(layerTex[0].texturePosition[0], layerTex[0].texturePosition[1]);
    glVertex3f(-width, height, 0.0f);
    glTexCoord2f(layerTex[0].texturePosition[2], layerTex[0].texturePosition[3]);
    glVertex3f(width, height, 0.0f);
    glTexCoord2f(layerTex[0].texturePosition[4], layerTex[0].texturePosition[5]);
    glVertex3f(width, -height, 0.0f);
    glTexCoord2f(layerTex[0].texturePosition[6], layerTex[0].texturePosition[7]);
    glVertex3f(-width, -height, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Paddle::drawGunLayer() const {
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, layerTex[1].textureProperties.texture);
    glColor4f(layerTex[1].textureProperties.glTexColorInfo[0],
              layerTex[1].textureProperties.glTexColorInfo[1],
              layerTex[1].textureProperties.glTexColorInfo[2],
              layerTex[1].textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(layerTex[1].texturePosition[0], layerTex[1].texturePosition[1]);
    glVertex3f(-width, height * 4, 0.0f);
    glTexCoord2f(layerTex[1].texturePosition[2], layerTex[1].texturePosition[3]);
    glVertex3f(width, height * 4, 0.0f);
    glTexCoord2f(layerTex[1].texturePosition[4], layerTex[1].texturePosition[5] - 0.01f);
    glVertex3f(width, height, 0.0f);
    glTexCoord2f(layerTex[1].texturePosition[6], layerTex[1].texturePosition[7] - 0.01f);
    glVertex3f(-width, height, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Paddle::draw(const float deltaTime) {
    if (dead) return;

    // ---FIXME
    // Wenn keine Spritesheet-Animationen vorhanden, muss auch kein Playback durchgeführt werden
    // Animation mit deltaTime aktualisieren
    texture.play(deltaTime);

    drawBase();

    if (hasGlueLayer) {
        layerTex[0].play(deltaTime);
        drawGlueLayer();
    }

    if (hasGunLayer) {
        layerTex[1].play(deltaTime);
        drawGunLayer();
    }
}

void Paddle::setGlueLayer(const bool enabled) {
    hasGlueLayer = enabled;
}

void Paddle::setGunLayer(const bool enabled) {
    hasGunLayer = enabled;
}

void Paddle::update(const float deltaTime) {
    updateGrowth(deltaTime);
    // Paddle bewegt sich nicht automatisch, sondern wird durch Benutzereingaben gesteuert
    // Bei Paddle keine automatische Bewegung durch MovingObject
    // daher kein MovingObject::update(deltaTime) aufrufen
}

// set Speed ?

void Paddle::moveTo(const float targetX, const float deltaTime) {
    // Unterscheidung zwischen direkter Positionierung und geschwindigkeitsbasierter Bewegung
    const bool isMouseInput = std::abs(targetX - pos_x) > 0.1f;

    if (isMouseInput) {
        // Mauseingabe: Direkte Positionierung mit Dämpfung
        constexpr float mouseSpeed = 8.0f;
        pos_x += (targetX - pos_x) * mouseSpeed * deltaTime;
    } else {
        // Richtung bestimmen (links/rechts)
        const float direction = (targetX > pos_x) ? 1.0f : -1.0f;
        // Nur bewegen, wenn Differenz vorhanden
        if (targetX != pos_x) {
            constexpr float keyboardSpeed = 20.0f;
            pos_x += direction * keyboardSpeed * deltaTime;

            // Vermeide Überschießen
            if ((direction > 0 && pos_x > targetX) ||
                (direction < 0 && pos_x < targetX)) {
                pos_x = targetX;
            }
        }
    }
}

void Paddle::onSizeChanged() {
    // Aktualisiere Kollisionspunkte wenn die Größe sich ändert
    // Einfache rechteckige Kollisionsform
    collisionPoints.clear();
    collisionPoints = {
        pos_x - width, pos_y + height, // oben links
        pos_x + width, pos_y + height, // oben rechts
        pos_x + width, pos_y - height, // unten rechts
        pos_x - width, pos_y - height // unten links
    };
}

const std::vector<float> *Paddle::getCollisionPoints() const {
    // Aktualisiere die Punkte mit aktuellen Positionen
    if (collisionPoints.empty() || collisionPoints.size() < 8) {
        collisionPoints.resize(8);
    }

    collisionPoints[0] = pos_x - width;
    collisionPoints[1] = pos_y + height;
    collisionPoints[2] = pos_x + width;
    collisionPoints[3] = pos_y + height;
    collisionPoints[4] = pos_x + width;
    collisionPoints[5] = pos_y - height;
    collisionPoints[6] = pos_x - width;
    collisionPoints[7] = pos_y - height;

    return &collisionPoints;
}

void Paddle::onCollision(ICollideable *other, float hitX, float hitY) {

    EventData data;
    data.posX = hitX;
    data.posY = hitY;
    data.sender = this;
    data.target = other;

    switch (other->getCollisionType()) {
        case static_cast<int>(CollisionType::Ball):
            eventManager->emit(GameEvent::BallHitPaddle, data);
            break;
        case static_cast<int>(CollisionType::PowerUp):
            eventManager->emit(GameEvent::PowerUpCollected, data);
            break;
        case static_cast<int>(CollisionType::BorderLeft):
            eventManager->emit(GameEvent::PaddleHitLeftBorder, data);
            break;
        case static_cast<int>(CollisionType::BorderRight):
            eventManager->emit(GameEvent::PaddleHitRightBorder, data);
            break;
        default: break;
    }
}

int Paddle::getCollisionType() const {
    return static_cast<int>(CollisionType::Paddle);
}
