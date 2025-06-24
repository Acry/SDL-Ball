# PlayerManager

The PlayerManager is responsible for managing the player settings and state in the game. It controls the paddle, lives,
coins, and other player-related attributes. The PlayerManager should not be responsible for initializing the player
directly; instead, it should retrieve the necessary settings from the GameManager, via SettingsManager, and pass them to
the Player class.

lives
coins
powerups
speed

Spawn

Despawn

Textures:
Dein bestehender Ansatz mit den separaten Properties für jede Schicht ist sehr praktisch und leicht verständlich.

ManagedObjects:
Da der PaddleManager nur ein Paddle verwalten soll, könnte der Vector für managedObjects auf ein einzelnes Element
beschränkt werden, aber das Beibehalten für Konsistenz mit anderen Managern ist auch sinnvoll.

Collisions:

## Old code explanation

Der Player wird hauptsächlich durch die globale Struktur player_struct player repräsentiert, die verschiedene
Spielerattribute wie Score, Level, Lives und Powerups enthält.

In initNewGame() werden die Grundwerte des Spielers zurückgesetzt
In resetPlayerPowerups() werden die Powerups zurückgesetzt
An verschiedenen Stellen im Hauptcode werden Player-Attribute aktualisiert (z.B. Score, Lives)
Das Paddle wird durch die paddle_class repräsentiert, die direkt in der main-Funktion instanziiert wird:
paddle_class paddle;
paddle.tex = texPaddleBase;
paddle.layerTex = texPaddleLayers;

Die Steuerung des Paddles erfolgt über die controllerClass:
controllerClass control(&paddle, &bullet, &bMan);

controllerClass
Diese Klasse verwaltet die Eingabe (Tastatur, Maus, Joystick) und bewegt das Paddle entsprechend. Kollisionsabfragen
zwischen Ball und Paddle werden in der padcoldet()-Funktion durchgeführt.

Die Player- und Paddle-Verwaltung ist mehr in der Hauptschleife verstreut und verwendet globale Variablen und direkte
Referenzen.

## collision detection

Die padcoldet()-Funktion erkennt Kollisionen zwischen dem Ball und dem Paddle durch folgende Schritte:
Sie prüft zunächst durch Bounding-Box-Tests, ob der Ball überhaupt in der Nähe des Paddles ist
Wenn ja, prüft sie für 32 Punkte auf dem Ball-Umfang, ob diese mit dem Paddle kollidieren

Winkelberechnung beim Aufprall des Balls auf das Paddle interessierst. Diese Funktionalität ist in der
bounceOffAngle-Funktion implementiert, die im Code jedoch nicht vollständig sichtbar ist.

b.setangle(bounceOffAngle(p.width, p.posx, b.posx));

Der Berechnungslogik nach zu urteilen, nimmt die Funktion bounceOffAngle drei Parameter:
Die Breite des Paddles (p.width)
Die X-Position des Paddles (p.posx)
Die X-Position des Balls (b.posx)

Der Berechnungslogik nach zu urteilen, nimmt die Funktion bounceOffAngle drei Parameter:
Die Breite des Paddles (p.width)
Die X-Position des Paddles (p.posx)
Die X-Position des Balls (b.posx)
Die Funktion nutzt die relative Position des Balls zum Paddle, um den Abprallwinkel zu bestimmen. Je nachdem, wo der
Ball auf dem Paddle auftrifft, wird ein anderer Winkel berechnet. Dies ist ein klassischer Ansatz in Breakout-Spielen,
bei dem:

Treffer in der Mitte zu einem steileren Winkel (nahe 90°) führen
Treffer am Rand zu einem flacheren Winkel führen
Der Code verwendet 1,571 Radianten als Ausgangspunkt (fast π/2 oder 90 Grad), und die tatsächliche Berechnung
modifiziert diesen Wert basierend auf der Auftreffposition.

Der Abprallwinkel hängt also direkt davon ab, wo der Ball auf dem Paddle trifft, wodurch der Spieler die
Bewegungsrichtung des Balls kontrollieren kann.

float bounceOffAngle(GLfloat width, GLfloat posx, GLfloat hitx)
{
return ( (BALL_MAX_DEGREE/(width*2.0))*(posx+width-hitx) + BALL_MIN_DEGREE );
}

## Paddle Bounce Angle Calculation

```c++
float bounceOffAngle(float paddleWidth, float paddlePosX, float ballPosX) {
    // Berechne die relative Position des Aufprallpunkts auf dem Paddel
    // 0.0 = Mitte, -1.0 = ganz links, 1.0 = ganz rechts
    float relativeIntersection = (paddlePosX - ballPosX) / paddleWidth;
    
    // Begrenze den Wert auf den Bereich [-1.0, 1.0]
    relativeIntersection = std::clamp(relativeIntersection, -1.0f, 1.0f);
    
    // Bestimme den Abprallwinkel
    // Pi/2 (90 Grad) ist direkt nach oben
    // Pi/6 (30 Grad) ist der maximale Winkel zur Seite
    const float maxAngle = M_PI / 6.0f; // 30 Grad
    float angle = M_PI / 2.0f + (relativeIntersection * maxAngle);
    
    return angle;
}
```

## auto-paddle

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

## convex collision paddle bounce angle

```c++
float calculateConvexPaddleBounceAngle(const Paddle& paddle, const Ball& ball) {
    // Relative Position des Auftreffpunkts zum Paddlezentrum (-1.0 bis 1.0)
    float paddleCenterX = paddle.getPosition().x + paddle.getWidth() / 2.0f;
    float ballCenterX = ball.getPosition().x + ball.getWidth() / 2.0f;
    float relativePos = (ballCenterX - paddleCenterX) / (paddle.getWidth() / 2.0f);
    
    // Begrenze auf den gültigen Bereich
    relativePos = std::clamp(relativePos, -1.0f, 1.0f);
    
    // Konfigurationsparameter
    const float MAX_BOUNCE_ANGLE = glm::radians(75.0f);
    const float CONVEXITY_FACTOR = 0.8f;  // Stärke der Wölbung (0 = flach, 1 = stark gewölbt)
    
    // Nicht-lineares Mapping durch quadratische oder kubische Funktion
    // für konvexes Verhalten
    float adjustedPos = relativePos * std::abs(relativePos) * CONVEXITY_FACTOR 
                      + relativePos * (1.0f - CONVEXITY_FACTOR);
    
    // Berechne den Abprallwinkel basierend auf der angepassten Position
    // 90° ist direkt nach oben, negative Werte gehen nach links, positive nach rechts
    return glm::radians(90.0f) - adjustedPos * MAX_BOUNCE_ANGLE;
}
```

Für die konvexe Paddle-Kollision hast du grundsätzlich zwei Optionen:
Option 1: Mathematische Simulation ohne separate Kollisionspunkte
Option 2: Physikalische Kollisionspunkte für komplexere Formen
