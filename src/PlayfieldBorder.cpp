// PlayfieldBorder.cpp
#include <epoxy/gl.h>

#include "config.h"
#include "PlayfieldBorder.h"

PlayfieldBorder::PlayfieldBorder(const Side side, const texture &tex, EventManager *eventManager)
    : GameObject(tex), side(side), eventManager(eventManager) {
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
    createDisplayList();
}

void PlayfieldBorder::createDisplayList() {
    if (displayList != 0) {
        glDeleteLists(displayList, 1);
    }

    displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureProperties.id);
    glColor4f(textureProperties.textureColor[0],
              textureProperties.textureColor[1],
              textureProperties.textureColor[2],
              textureProperties.textureColor[3]);
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

void PlayfieldBorder::draw() const {
    if (displayList != 0) {
        glCallList(displayList);
    }
}

void PlayfieldBorder::onCollision(const ICollideable *other, const float hitX, const float hitY) {
    switch (other->getCollisionType()) {
        case CollisionType::Ball: {
            EventData data{};
            data.posX = hitX;
            data.posY = hitY;
            data.sender = this;
            data.target = dynamic_cast<const GameObject *>(other);

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

        case CollisionType::Paddle: {
            EventData data{};
            data.posX = hitX;
            data.posY = hitY;
            data.sender = this;
            data.target = dynamic_cast<const GameObject *>(other);

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

CollisionType PlayfieldBorder::getCollisionType() const {
    switch (side) {
        case Side::Left: return CollisionType::BorderLeft;
        case Side::Right: return CollisionType::BorderRight;
        case Side::Top: return CollisionType::BorderTop;
        default: return CollisionType::None;
    }
}
