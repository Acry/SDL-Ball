// In difficulty_settings.h
#pragma once
#include <epoxy/gl.h>

namespace DifficultySettings {
    // Ballgeschwindigkeiten für verschiedene Schwierigkeitsstufen
    namespace BallSpeed {
        constexpr GLfloat EASY = 0.7f;
        constexpr GLfloat NORMAL = 1.3f;
        constexpr GLfloat HARD = 1.6f;
    }

    // Maximale Ballgeschwindigkeiten
    namespace MaxBallSpeed {
        constexpr GLfloat EASY = 1.5f;
        constexpr GLfloat NORMAL = 2.2f;
        constexpr GLfloat HARD = 3.0f;
    }

    // Geschwindigkeitserhöhung bei Brick-Kollision
    namespace HitBrickInc {
        constexpr GLfloat EASY = 0.0025f;
        constexpr GLfloat NORMAL = 0.003f;
        constexpr GLfloat HARD = 0.004f;
    }

    // Geschwindigkeitsänderung bei Paddle-Kollision
    namespace HitPaddleInc {
        constexpr GLfloat EASY = -0.001f;
        constexpr GLfloat NORMAL = -0.0005f;
        constexpr GLfloat HARD = -0.0007f;
    }

    // Prozentuale Geschwindigkeitsänderungen
    namespace SpeedUp {
        constexpr GLfloat EASY = 10.0f;
        constexpr GLfloat NORMAL = 20.0f;
        constexpr GLfloat HARD = 30.0f;
    }
}
