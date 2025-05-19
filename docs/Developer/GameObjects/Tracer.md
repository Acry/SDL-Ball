# Tracer

Der Tracer synchronisiert sich nicht mit dem Status des Balls.

Das Problem:
1. Der Tracer hat keinen eigenen `active`-Status als Gesamtobjekt
2. Er hat stattdessen ein Array von aktiven Partikeln (`active[100]`)
3. Wenn der Ball inaktiv wird, werden die Tracer-Partikel weiterhin gezeichnet

## Hier ist eine Lösungsidee:

```cpp
// In Tracer.h eine neue Variable hinzufügen
class Tracer {
    // Bestehende Variablen...
    
public:
    // Neue Variable für den Gesamtstatus
    bool active{true};
    
    // Bestehende Methoden...
};
```

```cpp
// In Tracer.cpp die draw-Methode anpassen
void Tracer::draw(float deltaTime) {
    // Wenn Tracer inaktiv ist, nichts zeichnen
    if (!active) return;
    
    // Bestehender Code...
}

// In Tracer.cpp die update-Methode anpassen
void Tracer::update(const GLfloat nx, const GLfloat ny) {
    // Wenn Tracer inaktiv ist, keine Updates
    if (!active) return;
    
    // Bestehender Code...
}
```

Dann in der Ball-Klasse die Synchronisation implementieren:

```cpp
// In Ball::update()
void Ball::update(const float deltaTime) {
    // Wenn der Ball nicht aktiv ist
    if (!active) {
        // Tracer deaktivieren
        tracer.active = false;
        return;
    }

    // Tracer aktivieren, wenn eyeCandy an ist
    tracer.active = eyeCandy;

    // Restlicher Code...
    
    // Beim Aktualisieren des Tracers (am Ende der Methode)
    if (eyeCandy && (xvel != 0.0f || yvel != 0.0f)) {
        tracer.update(pos_x, pos_y);
    }
}

// In Ball::init() oder reset()-Methode
void Ball::init() {
    // Bestehender Code...
    
    // Tracer zurücksetzen
    tracer.active = active && eyeCandy;
    
    // Alle Partikel zurücksetzen
    for (int i = 0; i < tracer.len; i++) {
        tracer.active[i] = false; // Falls du Zugriff auf die Partikel benötigst
    }
    
    // Position aktualisieren
    tracer.update(pos_x, pos_y);
}
```

Diese Änderungen ermöglichen, dass:
1. Der Tracer als Ganzes aktiviert/deaktiviert wird
2. Der Tracer-Status mit dem Ball-Status synchronisiert wird
3. Bei Ball-Reset auch alle Tracer-Partikel zurückgesetzt werden

Für eine saubere Implementierung wäre eigentlich ein komplettes Refactoring des Tracers sinnvoll, da das aktuelle Design mit den festen Arrays etwas veraltet ist. Eine modernere Lösung würde Vektoren und objektorientiertes Design für Partikel verwenden.