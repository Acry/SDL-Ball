# physics

Box2D
https://github.com/erincatto/box2d

Die Elastizität beschreibt, wie viel kinetische Energie bei einer Kollision erhalten bleibt:

```c++
void handleCollision(Ball& ball1, Ball& ball2) {
    Vector3 collisionNormal = (ball2.position - ball1.position).normalized();
    
    // Relativgeschwindigkeit entlang der Kollisionsnormale
    float relativeVelocity = (ball2.velocity - ball1.velocity).dot(collisionNormal);
    
    // Bei sich voneinander entfernenden Objekten keine Kollisionsbehandlung
    if (relativeVelocity > 0) return;
    
    // Elastizitätskoeffizient (1 = perfekt elastisch, 0 = unelastisch)
    float elasticity = ball1.material.elasticity * ball2.material.elasticity;
    
    // Impulsberechnung unter Berücksichtigung der Elastizität
    float j = -(1 + elasticity) * relativeVelocity / 
             (1/ball1.mass + 1/ball2.mass);
    
    // Geschwindigkeitsänderungen anwenden
    ball1.velocity -= j * collisionNormal / ball1.mass;
    ball2.velocity += j * collisionNormal / ball2.mass;
}
```

Die Brownsche Bewegung kann für natürlich wirkende, zufällige Bewegungen von Objekten implementiert werden:
Enemies

```c++
void applyBrownianMotion(Ball& ball, float deltaTime) {
    // Stärke der Brownschen Bewegung
    float brownianStrength = 0.5f;
    
    // Zufallsvektor mit Gauß-Verteilung
    Vector3 randomForce(
        getGaussianRandom(-1.0f, 1.0f),
        getGaussianRandom(-1.0f, 1.0f),
        getGaussianRandom(-1.0f, 1.0f)
    );
    
    // Normalisieren und mit Stärke und Zeitschritt skalieren
    randomForce.normalize();
    randomForce *= brownianStrength * deltaTime;
    
    // Kraft auf die Geschwindigkeit anwenden
    ball.velocity += randomForce / ball.mass;
}

// Hilfsfunktion für gauß-verteilte Zufallszahlen
float getGaussianRandom(float min, float max) {
    // Box-Muller-Transformation für Normalverteilung
    float u1 = rand() / (float)RAND_MAX;
    float u2 = rand() / (float)RAND_MAX;
    
    float z0 = sqrt(-2.0f * log(u1)) * cos(2.0f * M_PI * u2);
    
    // Auf gewünschten Bereich skalieren
    return min + (max - min) * (z0 * 0.1f + 0.5f);
}
```

Für Flocking (Schwarmverhalten) können die klassischen Boids-Regeln implementiert werden:

```c++
void applyFlockingBehavior(std::vector<Ball>& balls, float deltaTime) {
for (Ball& ball : balls) {
Vector3 separation(0, 0, 0);   // Abstoßung bei zu großer Nähe
Vector3 alignment(0, 0, 0);    // Ausrichtung nach Nachbarn
Vector3 cohesion(0, 0, 0);     // Zusammenhalt des Schwarms

        int neighborCount = 0;
        
        // Nachbarn finden und Kräfte berechnen
        for (const Ball& other : balls) {
            if (&other == &ball) continue;
            
            Vector3 toOther = other.position - ball.position;
            float distance = toOther.length();
            
            if (distance < 10.0f) { // Nahbereich für Separation
                separation -= toOther.normalized() / distance;
            }
            
            if (distance < 30.0f) { // Einflussbereich für Alignment und Cohesion
                alignment += other.velocity;
                cohesion += other.position;
                neighborCount++;
            }
        }
        
        // Kräfte normalisieren und gewichten
        if (neighborCount > 0) {
            alignment /= neighborCount;
            cohesion /= neighborCount;
            cohesion = (cohesion - ball.position) * 0.1f;
        }
        
        // Kräfte auf Ball anwenden
        Vector3 flockingForce = separation * 1.5f + alignment * 1.0f + cohesion * 1.0f;
        flockingForce.normalize();
        flockingForce *= 2.0f; // Stärke der Flocking-Kraft
        
        ball.velocity += flockingForce * deltaTime;
    }
}
```

nun, ich habe dabei eher an gegner gedacht, die dann spawnen.

wie bei arkanoid und bombjack