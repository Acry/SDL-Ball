# Überarbeitung von BallManager.cpp

Der alte BallManager verwendete drei Texturen:
tex[0] - Die normale Ball-Textur
tex[1] - Die Feuerball-Textur (für explosive Bälle)
tex[2] - Die Tracer-Textur (für den Schweif/Effekt hinter dem Ball)

## Aufgaben

// Ball-Verwaltung

- Ball-Objektverwaltung: Erstellen, Speichern und Entfernen von Ball-Objekten
- Texturen und Animationen: Zuordnung der richtigen Textur (normal/explodierend)
- Ball-Zustände verwalten: Status wie aktiv/inaktiv verfolgen (Nachhalten der aktiven Bälle)
- Kollisionslogik an den CollisionManager delegieren: keine direkte Kollisionserkennung
  Ball-Logik vom Anzeigeteil trennen: Ball-Klasse nur für Darstellung

Festlegung von Ball-Eigenschaften
Zuweisung der passenden Textur (normal/explosiv)
Größe und Geschwindigkeit der Bälle festlegen

// Ball-Aktionen
Multiplikation von Bällen (für Powerups)
Loslösen von klebenden Bällen
Anwendung von Powerup-Effekten

Status-Management
Durchschnittsgeschwindigkeit der Bälle berechnen
Anzahl aktiver Bälle ermitteln

// Update & Draw
// Hilfsfunktionen
// Delegationen an CollisionManager

Instanziert Bälle
Vergibt Texturen

struct BallState {
bool active{true}; // Ist der Ball aktiv?
bool explosive{false}; // Ist der Ball explosiv?
bool glued{false}; // Am Paddle festgeklebt?
float gluedX{0.0f}; // Relative X-Position auf dem Paddle
float xvel{0.0f};
float yvel{0.0f};
float angle{0.0f}; // Bewegungswinkel
float velocity{0.0f}; // Geschwindigkeit
};
std::vector<BallState> ballStates;

    // Zugriff auf Bälle
    Ball* getBallAt(size_t index);
    std::vector<Ball*> getBalls();

size_t getActiveBallCount() const;
float getAverageBallSpeed() const;

calculateAverageBallSpeed
calculateVelocityFromAngle
updateBallPositions
handleBallBorderCollision
handleBallPaddleCollision
handleBallBrickCollision

void setAngle(GLfloat o);

void setSpeed(GLfloat v, GLfloat maxSpeed = 1.5f);

void setSize(GLfloat s);

handleBallLost

createInitialBall

spawnBall (powerup)
void clearAllBalls();

void releaseGluedBalls();
void applyPowerup(int powerupType);
void launchBallFromPaddle(size_t ballIndex);
void setBallAngle(size_t ballIndex, float angle);
void setBallSpeed(size_t ballIndex, float speed);
void setExplosiveBall(size_t ballIndex, bool explosive);

calculate center

## Collsion

std::vector<ICollideable*> getCollideables() const;

### Advanced colission

mutable std::vector<float> collisionPoints;
