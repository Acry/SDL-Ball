# Auto-play

```c++
// Auto-Paddle-Mechanik
if (autoPaddle) {
    // Ballflugbahn vorhersagen statt nur aktuelle Position zu verfolgen
    float predictedX = ball.pos_x;
    float predictedY = ball.pos_y;
    float velX = ball.xvel;
    float velY = ball.yvel;

    // Nur für aufsteigende Bälle die Vorhersage berechnen
    if (velY > 0) {
        // Einfache Vorhersage: Wie weit fliegt der Ball horizontal,
        // bis er die Paddlehöhe erreicht?
        if (float timeToReachPaddle = (paddle.pos_y - predictedY) / velY; timeToReachPaddle > 0) {
            predictedX += velX * timeToReachPaddle;
        }
    }

    // Kleiner zufälliger Fehler für realistischeres Verhalten (nur alle 0.5 Sekunden neu berechnen)
    static float lastErrorTime = 0.0f;
    static float errorOffset = 0.0f;

    if (float autoPaddleTime = 0.0f; autoPaddleTime - lastErrorTime > 0.5f) {
        lastErrorTime = autoPaddleTime;
        errorOffset = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
    }

    // Paddlemitte zur vorhergesagten Ballposition bewegen, mit kleinem Fehler
    float target = (predictedX + ball.width / 2.0f + errorOffset) - paddle.getWidth() / 2.0f;

    // Auf Spielfeldgrenzen beschränken
    if (target < -1.0f) target = -1.0f;
    if (target > 1.0f - paddle.getWidth()) target = 1.0f - paddle.getWidth();

    // Sanfte Bewegung zum Ziel
    float currentX = paddle.pos_x;
    float moveSpeed = 1.5f; // Anpassbarer Wert - höher = schnellere Reaktion
    float newX = currentX + (target - currentX) * moveSpeed * deltaTime * 10.0f;

    paddle.moveTo(newX, deltaTime);
}
```