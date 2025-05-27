#include "Speedometer.h"
#include <epoxy/gl.h>
#include "colors.h"
#include "game_state.h"
#include "config.h"

void Speedometer::draw() {
    float targetHeight;
    switch (player.difficulty) {
        case EASY:
            targetHeight = (var.averageBallSpeed / fixed_difficulty.maxballspeed[EASY]) * 2.0f;
            break;
        case NORMAL:
            targetHeight = (var.averageBallSpeed / fixed_difficulty.maxballspeed[NORMAL]) * 2.0f;
            break;
        case HARD:
            targetHeight = (var.averageBallSpeed / fixed_difficulty.maxballspeed[HARD]) * 2.0f;
            break;
        default:
            targetHeight = 0.0f;
    }

    glLoadIdentity();
    constexpr float width = 0.03f;
    glTranslatef(1.0f, -1.0f, 0.0f);
    glBegin(GL_QUADS);
    glColor4f(GL_FULL_GREEN);
    glVertex3f(0.0f, targetHeight, 0.0f);
    glVertex3f(-width * 2.0f, targetHeight, 0.0f);
    glColor4f(GL_WHITE);
    glVertex3f(-width * 2.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
}
