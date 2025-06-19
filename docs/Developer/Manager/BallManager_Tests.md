# BallManager Tests

## Side-View:

Elements of a Pinball Game in Breakout

Der Ballmanager hat das Potenzial für ein Pinball-Spiel

Was die Kollisionserkennung in einer Röhre betrifft, lässt sich dies mathematisch relativ elegant lösen:

Bei einer Röhre handelt es sich um einen zylindrischen Körper mit einer inneren Oberfläche
Die Kollision des Balls mit der Röhrenwand kann durch den Abstand zwischen der Kugelposition und der Röhrenachse
bestimmt werden

```c++
bool checkTubeCollision(const Ball& ball, const Tube& tube) {
    // Berechne den nächsten Punkt auf der Röhrenachse zum Ballmittelpunkt
    Vector3 tubeDirection = tube.endPoint - tube.startPoint;
    tubeDirection.normalize();
    
    // Vektor vom Röhrenanfang zum Ball
    Vector3 ballVector = ball.position - tube.startPoint;
    
    // Projektion des Ballvektors auf die Röhrenachse
    float projection = ballVector.dot(tubeDirection);
    
    // Prüfe, ob Ball innerhalb der Röhrenlänge liegt
    if (projection < 0 || projection > tube.length) {
        return false; // Ball ist außerhalb der Röhrenlänge
    }
    
    // Punkt auf der Röhrenachse, der dem Ball am nächsten ist
    Vector3 closestPoint = tube.startPoint + tubeDirection * projection;
    
    // Abstand zwischen Ball und Röhrenachse
    float distance = (ball.position - closestPoint).length();
    
    // Kollision tritt auf, wenn der Abstand kleiner als Röhrenradius-Ballradius ist
    return distance < (tube.innerRadius - ball.radius);
}
```

Diese Implementierung deckt die grundlegende Kollisionserkennung ab. Für ein vollständiges Pinball-Spiel müsstest du
zusätzlich die Reaktion auf die Kollision (Abprallen) implementieren und weitere Elemente wie Flipper, Bumper und Rampen
hinzufügen.

## Rotation

Die Rotation des Balls kann durch die Anwendung eines Rotationsvektors oder einer Rotationsmatrix erfolgen. Hier ist ein
einfaches Beispiel, wie du die Rotation eines Balls um eine Achse implementieren kannst:

Ballrotation: Die Dokumentation zeigt, wie Ballrotationen mittels Quaternionen basierend auf der linearen
Geschwindigkeit implementiert werden können.

```c++
void updateBallRotation(Ball& ball, float deltaTime) {
    // Winkelgeschwindigkeit aus linearer Geschwindigkeit und Ballradius
    float speed = ball.velocity.length();
    if (speed > 0.001f) {
        // Rotationsachse: Senkrecht zu Geschwindigkeit und Aufwärtsvektor
        Vector3 rotationAxis = ball.velocity.cross(Vector3(0, 1, 0));
        if (rotationAxis.lengthSquared() < 0.001f) {
            rotationAxis = Vector3(1, 0, 0); // Fallback-Achse, wenn Geschwindigkeit vertikal
        }
        rotationAxis.normalize();
        
        // Winkelgeschwindigkeit = lineare Geschwindigkeit / Radius
        float angularSpeed = speed / ball.radius;
        
        // Quaternion für die Rotation erstellen
        Quaternion rotationDelta = Quaternion::fromAxisAngle(rotationAxis, angularSpeed * deltaTime);
        
        // Rotation anwenden
        ball.orientation = rotationDelta * ball.orientation;
        ball.orientation.normalize(); // Normalisieren, um numerische Fehler zu vermeiden
    }
}
```

## Gravitation und Rollwiderstand

Die Gravitation und der Rollwiderstand können durch die Anwendung von Kräften auf den Ball simuliert werden. Hier ist
ein einfaches Beispiel, wie du diese Kräfte in der Update-Schleife des Balls berücksichtigen kannst:

```c++
void applyPhysics(Ball& ball, float deltaTime) {
    // Gravitation
    ball.velocity.y -= 9.81f * deltaTime;
    
    // Rollwiderstand auf horizontalen Flächen
    if (ball.isOnSurface) {
        Vector3 horizontalVelocity(ball.velocity.x, 0.0f, ball.velocity.z);
        float horizontalSpeed = horizontalVelocity.length();
        
        if (horizontalSpeed > 0.001f) {
            // Rollwiderstandskoeffizient abhängig von Untergrund und Ball
            float rollResistance = ball.material.rollResistance * ball.currentSurface.rollResistance;
            
            // Widerstandskraft berechnen
            float resistanceForce = rollResistance * ball.mass * 9.81f;
            
            // Maximale Verlangsamung in diesem Frame
            float maxSlowdown = resistanceForce * deltaTime / ball.mass;
            
            // Verlangsamung anwenden (aber nicht mehr als aktuelle Geschwindigkeit)
            float slowdownFactor = std::min(maxSlowdown / horizontalSpeed, 1.0f);
            ball.velocity.x -= ball.velocity.x * slowdownFactor;
            ball.velocity.z -= ball.velocity.z * slowdownFactor;
        }
    }
}
```

## Bezier- oder Spline-Kurven

Die Kollisionserkennung für komplexe Formen kann durch die Verwendung von mathematischen Kurven und Segmentierung
verbessert werden. Hier sind einige Ansätze:
Bezier- oder Spline-Kurven
Für Röhren mit Kurven oder komplexen Formen können Bezier- oder Spline-Kurven verwendet werden, um den Pfad zu
beschreiben. Die Kollisionserkennung erfolgt durch Berechnung des kürzesten Abstands zwischen dem Ball und der Kurve.

```c++
bool checkCurveCollision(const Ball& ball, const Curve& curve) {
    // Berechne den nächsten Punkt auf der Kurve zum Ballmittelpunkt
    Vector3 closestPoint = curve.getClosestPoint(ball.position);

    // Abstand zwischen Ball und Kurve
    float distance = (ball.position - closestPoint).length();

    // Kollision tritt auf, wenn der Abstand kleiner als Kurvenradius-Ballradius ist
    return distance < (curve.radius - ball.radius);
}
```

Segmentierung
Komplexe Formen können in kleinere Segmente unterteilt werden, z.B. Linien, Kreisbögen oder Polygone. Die
Kollisionserkennung wird dann für jedes Segment separat durchgeführt.

```c++
bool checkSegmentCollision(const Ball& ball, const Segment& segment) {
    // Berechne den kürzesten Abstand zwischen Ball und Segment
    Vector3 closestPoint = segment.getClosestPoint(ball.position);

    // Abstand zwischen Ball und Segment
    float distance = (ball.position - closestPoint).length();

    // Kollision tritt auf, wenn der Abstand kleiner als Segmentradius-Ballradius ist
    return distance < (segment.radius - ball.radius);
}
```

Bounding Volumes
Verwende Bounding Volumes wie Achsenausgerichtete Begrenzungsboxen (AABB) oder Kugeln, um die Kollisionserkennung zu
vereinfachen. Erst wenn eine Kollision mit dem Bounding Volume erkannt wird, wird die detaillierte Kollisionserkennung
durchgeführt.

```c++
bool checkBoundingVolumeCollision(const Ball& ball, const BoundingVolume& volume) {
    // Prüfe, ob Ball innerhalb des Bounding Volumes liegt
    return volume.contains(ball.position);
}
```

## Siehe auch

Dynamik und Kinetik

Kurven: Für komplexere Röhrengeometrien wie Kurven können Bezier- oder Splinekurven verwendet werden, um den Pfad
mathematisch zu beschreiben und Kollisionen entlang dieser Pfade zu berechnen.

Impulserhaltung: Bei Kollisionen werden die Geschwindigkeiten nach den Gesetzen der Elastizität und Impulserhaltung neu
berechnet. Die Massen der Bälle werden proportional zur dritten Potenz ihres Radius bestimmt.

Für komplexe Formen und dynamische Szenarien können Physik-Engines wie Bullet oder Box2D verwendet werden, die
fortgeschrittene Algorithmen für Kollisionserkennung und -behandlung bieten.

Diese Ansätze können kombiniert werden, um die Effizienz und Genauigkeit der Kollisionserkennung zu verbessern.

../Physics & Timing/Collision.md
../Physics & Timing/Physcis.md
../Physics & Timing/Timing.md
