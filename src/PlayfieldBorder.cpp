// PlayfieldBorder.cpp
#include <epoxy/gl.h>

#include "config.h"
#include "PlayfieldBorder.h"

PlayfieldBorder::PlayfieldBorder(const Side side, const texture &tex, EventManager *eventManager)
    : GameObject(tex), side(side), eventManager(eventManager) {
    if (side == Side::Top) {
        eventManager->addListener(GameEvent::LevelLoaded,
                                  [this](const LevelData &data) { handleLevelLoaded(data); }, this);
    }
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


    const float left = pos_x;
    const float right = pos_x + width;
    const float bottom = pos_y;
    const float top = pos_y + height;

    if (side == Side::Top) {
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(left, bottom, 0.1f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(right, bottom, 0.1f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(right, top, 0.1f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(left, top, 0.1f);
        glEnd();

        // Füllfläche zwischen ursprünglichem Top und aktueller Border
        if (pos_y <= 1.0f && pos_y < PLAYFIELD_TOP_BORDER) {
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
            // Obere Kante: Schwarz
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            glVertex3f(left, PLAYFIELD_TOP_BORDER, 0.05f);
            glVertex3f(right, PLAYFIELD_TOP_BORDER, 0.05f);
            // Untere Kante: grau
            glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
            glVertex3f(right, top, 0.05f);
            glVertex3f(left, top, 0.05f);
            glEnd();
            glDisable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
        }
    } else if (side == Side::Left) {
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(left, bottom, 0.1f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(right, bottom, 0.1f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(right, top, 0.1f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(left, top, 0.1f);
        glEnd();
    } else if (side == Side::Right) {
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(left, bottom, 0.1f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(right, bottom, 0.1f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(right, top, 0.1f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(left, top, 0.1f);
        glEnd();
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void PlayfieldBorder::update(const float deltaTime) {
    if (side == Side::Top && dropSpeed > 0.0f) {
        dropTimerMs += deltaTime * 1000.0f;
        if (dropTimerMs >= dropSpeed) {
            pos_y -= BRICK_HEIGHT;
            dropTimerMs = 0.0f;
            createDisplayList();
        }
    }
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

void PlayfieldBorder::handleLevelLoaded(const LevelData &data) {
    if (side == Side::Top && data.dropSpeed > 0.0f) {
        dropSpeed = data.dropSpeed;
        dropTimerMs = 0.0f;
    } else {
        dropSpeed = 0.0f;
        dropTimerMs = 0.0f;
    }
    init();
}

CollisionType PlayfieldBorder::getCollisionType() const {
    switch (side) {
        case Side::Left: return CollisionType::BorderLeft;
        case Side::Right: return CollisionType::BorderRight;
        case Side::Top: return CollisionType::BorderTop;
        default: return CollisionType::None;
    }
}
