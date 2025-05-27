#include "Powerup.h"
#include "Constants.h"
#include <epoxy/gl.h>

Powerup::Powerup(EventManager *eventMgr) : eventManager(eventMgr), active(false) {
    width = 0.05f;
    height = 0.05f;
    yvel = -0.3f; // Powerups fallen immer nach unten
    xvel = 0.0f;
}

void Powerup::init() {
    texture.init();
    active = false;
}

void Powerup::update(float deltaTime) {
    if (!active) return;

    // Bewegungslogik
    pos_y += yvel * deltaTime;

    // Deaktivieren wenn aus dem Bildschirm gefallen
    if (pos_y < -1.2f) {
        deactivate();
    }
}

void Powerup::draw(float deltaTime) {
    if (!active) return;

    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture.play(deltaTime);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureProperties.texture);
    glColor4f(texture.textureProperties.glTexColorInfo[0],
              texture.textureProperties.glTexColorInfo[1],
              texture.textureProperties.glTexColorInfo[2],
              texture.textureProperties.glTexColorInfo[3]);

    glBegin(GL_QUADS);
    glTexCoord2f(texture.texturePosition[0], texture.texturePosition[1]);
    glVertex3f(-width, height, 0.0);
    glTexCoord2f(texture.texturePosition[2], texture.texturePosition[3]);
    glVertex3f(width, height, 0.0);
    glTexCoord2f(texture.texturePosition[4], texture.texturePosition[5]);
    glVertex3f(width, -height, 0.0);
    glTexCoord2f(texture.texturePosition[6], texture.texturePosition[7]);
    glVertex3f(-width, -height, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Powerup::activate(PowerupType powerupType, float posX, float posY) {
    type = powerupType;
    pos_x = posX;
    pos_y = posY;
    active = true;

    // Textur entsprechend des Powerup-Typs laden
    std::string texturePath;
    switch (type) {
        case POWERUP_GROW:
            texturePath = "data/gfx/powerup_grow.png";
            break;
        case POWERUP_SHRINK:
            texturePath = "data/gfx/powerup_shrink.png";
            break;
        case POWERUP_SPEED:
            texturePath = "data/gfx/powerup_speed.png";
            break;
        case POWERUP_SLOW:
            texturePath = "data/gfx/powerup_slow.png";
            break;
        case POWERUP_MULTI:
            texturePath = "data/gfx/powerup_multi.png";
            break;
        case POWERUP_EXTRA_LIFE:
            texturePath = "data/gfx/powerup_life.png";
            break;
        case POWERUP_EXPLOSIVE:
            texturePath = "data/gfx/powerup_explosive.png";
            break;
        default:
            texturePath = "data/gfx/powerup_default.png";
            break;
    }

    texture.load(texturePath);
}

void Powerup::deactivate() {
    active = false;
}
