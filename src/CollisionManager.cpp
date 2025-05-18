// CollisionManager.cpp
#include "CollisionManager.h"

#include "game_state.h"
#include "MathHelper.h"
#include "SettingsManager.h"

bool CollisionManager::checkCollision(const GameObject& obj1, const GameObject& obj2) {
    // Implementierung der AABB-Kollisionserkennung (Axis-Aligned Bounding Box)
    return (obj1.pos_x - obj1.width < obj2.pos_x + obj2.width &&
            obj1.pos_x + obj1.width > obj2.pos_x - obj2.width &&
            obj1.pos_y - obj1.height < obj2.pos_y + obj2.height &&
            obj1.pos_y + obj1.height > obj2.pos_y - obj2.height);
}

bool CollisionManager::checkBallPaddleCollision(Ball& ball, const Paddle& paddle, CollisionPoint& collisionPoint) {
    // Vereinfachte Prüfung, ob Ball in der Nähe des Paddles ist
    if (ball.pos_y < (paddle.pos_y + paddle.height) + ball.height &&
        ball.pos_y > paddle.pos_y - paddle.height) {
        if (ball.pos_x > paddle.pos_x - (paddle.width * 2.0) - ball.width &&
            ball.pos_x < paddle.pos_x + (paddle.width * 2.0) + ball.width) {

            // Detaillierte Kollisionsprüfung mit den Ball-Punkten
            float px = 0, py = 0;
            bool collision = false;
            int points = 0;

            for (int i = 0; i < 32; i++) {
                float x = ball.bsin[i];
                float y = ball.bcos[i];

                // Prüfe, ob Punkt im Paddle ist
                if (ball.pos_x + x > paddle.pos_x - paddle.width &&
                    ball.pos_x + x < paddle.pos_x + paddle.width) {
                    if (ball.pos_y + y < paddle.pos_y + paddle.height &&
                        ball.pos_y + y > paddle.pos_y - paddle.height) {
                        collision = true;
                        px += x;
                        py += y;
                        points++;
                    }
                }
            }

            if (collision && ball.yvel < 0) {
                // Kollisionspunkt berechnen
                px /= static_cast<float>(points);
                py /= static_cast<float>(points);

                collisionPoint.x = ball.pos_x + px;
                collisionPoint.y = ball.pos_y + py;

                // Ball-Position korrigieren
                ball.pos_y = paddle.pos_y + paddle.height + ball.height;

                // Abprallwinkel berechnen
                float angle = calculateBounceAngle(paddle.width, paddle.pos_x, ball.pos_x);
                ball.setangle(angle);

                return true;
            }
        }
    }

    return false;
}

float CollisionManager::calculateBounceAngle(float paddleWidth, float paddleX, float hitX) {
    // Berechne relativen Auftreffpunkt (-1 bis +1)
    float relativeX = (hitX - paddleX) / paddleWidth;

    // Begrenze den Wert
    if (relativeX > 1.0f) relativeX = 1.0f;
    if (relativeX < -1.0f) relativeX = -1.0f;

    // Berechne den Winkel im zulässigen Bereich
    return BALL_MIN_DEGREE + ((BALL_MAX_DEGREE) * (relativeX + 1.0f) / 2.0f);
}