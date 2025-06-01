#include <epoxy/gl.h>
#include "PlayfieldBorder.h"
#include "EventManager.h"
#include "config.h"

PlayfieldBorder::PlayfieldBorder(Side side, const SpriteSheetAnimation &tex, EventManager *eventMgr)
    : GameObject(eventMgr), side(side) {
    // eventMgr an GameObject-Konstruktor weitergeben
    this->texture = tex;
    init();
}

void PlayfieldBorder::init() {
    if (side == Side::Left) {
        pos_x = -1.0f;
        pos_y = -1.0f;
        width = PILLAR_WIDTH;
        height = 2.0f;
    } else if (side == Side::Right) {
        pos_x = 1.0f - PILLAR_WIDTH;
        pos_y = -1.0f;
        width = PILLAR_WIDTH;
        height = 2.0f;
    } else if (side == Side::Top) {
        pos_x = PLAYFIELD_LEFT_BORDER;
        pos_y = PLAYFIELD_TOP_BORDER; // Oberer Rand (außerhalb des sichtbaren Bereichs)
        width = 2.0f - PILLAR_WIDTH * 2;
        height = PILLAR_WIDTH;
    }
    // Erzeuge die DisplayList
    createDisplayList();
}

void PlayfieldBorder::draw(float) {
    if (dl)
        glCallList(dl);
}

void PlayfieldBorder::onCollision(ICollideable *other, const float hitX, const float hitY) {
    switch (other->getCollisionType()) {
        case static_cast<int>(CollisionType::Ball): {
            EventData data{};
            data.posX = hitX;
            data.posY = hitY;
            data.sender = this;
            data.target = dynamic_cast<const GameObject*>(other);

            switch (side) {
                case Side::Left:
                    eventManager->emit(GameEvent::BallHitLeftBorder, data);
                    break;
                case Side::Right:
                    eventManager->emit(GameEvent::BallHitRightBorder, data);
                    break;
                case Side::Top:
                    eventManager->emit(GameEvent::BallHitTopBorder, data);
                    break;
            }
        }
        break;

        case static_cast<int>(CollisionType::Paddle): {
            EventData data{};
            data.posX = hitX;
            data.posY = hitY;
            data.sender = this;
            data.target = dynamic_cast<const GameObject*>(other);

            if (side == Side::Left) {
                eventManager->emit(GameEvent::PaddleHitLeftBorder, data);
            } else {
                eventManager->emit(GameEvent::PaddleHitRightBorder, data);
            }
        }
        break;
        default: ;
    }
}

void PlayfieldBorder::createDisplayList() {
    if (dl) {
        glDeleteLists(dl, 1);
    }

    dl = glGenLists(1);
    glNewList(dl, GL_COMPILE);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
    glColor4f(texture.textureProperties.glTexColorInfo[0],
              texture.textureProperties.glTexColorInfo[1],
              texture.textureProperties.glTexColorInfo[2],
              texture.textureProperties.glTexColorInfo[3]);
    glBegin(GL_QUADS);

    if (side == Side::Left) {
        // Bottom-left corner.
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.1f);

        // Bottom-right corner.
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-1.0f + PILLAR_WIDTH, -1.0f, 0.1f);

        // Top-right corner.
        glTexCoord2f(1.0f, -1.0f);
        glVertex3f(-1.0f + PILLAR_WIDTH, 1.0f, 0.1f);

        // Top-left corner.
        glTexCoord2f(0.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, 0.1f);
    } else if (side == Side::Right) {
        // Bottom-left corner.
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f - PILLAR_WIDTH, -1.0f, 0.1f);

        // Bottom-right corner.
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.1f);

        // Top-right corner.
        glTexCoord2f(0.0f, -1.0f);
        glVertex3f(1.0f, +1.0f, 0.1f);

        // Top-left corner.
        glTexCoord2f(1.0f, -1.0f);
        glVertex3f(1.0f - PILLAR_WIDTH, +1.0f, 0.1f);
    } else if (side == Side::Top) {
        // Bottom-left corner.
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-1.0f + PILLAR_WIDTH, 1.0f, 0.1f);

        // Bottom-right corner.
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f - PILLAR_WIDTH, 1.0f, 0.1f);

        // Top-right corner.
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(1.0f - PILLAR_WIDTH, 1.0f + PILLAR_WIDTH, 0.1f);

        // Top-left corner.
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f + PILLAR_WIDTH, 1.0f + PILLAR_WIDTH, 0.1f);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEndList();
}

int PlayfieldBorder::getCollisionType() const {
    switch (side) {
        case Side::Left: return static_cast<int>(CollisionType::BorderLeft);
        case Side::Right: return static_cast<int>(CollisionType::BorderRight);
        case Side::Top: return static_cast<int>(CollisionType::BorderTop);
        default: return static_cast<int>(CollisionType::None);
    }
}
