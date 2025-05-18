# Ball.md

## Analyse des Ball-Objekts

### Aktuelle Ball-Implementierung

Der `Ball` ist ein zentrales Spielobjekt, das von `MovingObject` erbt. Es hat folgende Haupteigenschaften:

- Physikalisches Verhalten: Bewegung, Kollision mit Spielfeldbegrenzungen und Paddle
- Visuelle Effekte: Wachsen/Schrumpfen, Explosionsanimation, Tracer-Spur
- Spezielle Zustände:
    - `glued` (am Paddle festgeklebt)
    - `explosive` (Explosionszustand)
    - Ziel-/Hilfslinie bei `PO_AIM` und `PO_AIMHELP` Powerups

### Entkopplungsmöglichkeiten

### 1. Trennung der Rendering-Logik

Die `draw()`-Methode enthält zu viel Rendering-Code und Spiellogik. Dieser sollte in einem eigenen Renderer gekapselt werden.

### 2. Physik-Logik extrahieren

Kollisionserkennung (besonders `checkPaddleCollision()`) könnte in einen Physik-Manager oder Collision-Handler ausgelagert werden.

### 3. PowerUp-Effekte separieren

Powerup-Effekte wie Laser oder Zielhilfe sind stark mit dem Ball-Code verwoben, sollten aber eigentlich separate Zuständigkeiten sein.

### 4. Tracer als eigenständiges Objekt

Die Tracer-Funktionalität könnte vom Ball komplett getrennt werden.

### Empfehlungen für Refactoring

1. **Erstelle eine eigene `BallRenderer`-Klasse**
    - Übernimmt alle OpenGL-Rendering-Aufgaben
    - Ball selbst speichert nur noch Zustandsdaten

2. **Erstelle einen `CollisionManager`**
    - Prüft Kollisionen zwischen Spielobjekten
    - Berechnet Abprallwinkel und -verhalten

3. **PowerUp-Effekte zum `EffectManager` verschieben**
    - Lass den Ball-Code rein für Ball-Eigenschaften zuständig sein

4. **Clean Code**
    - Magische Zahlen durch Konstanten ersetzen
    - Bessere Benennung (z.B. `rad` -> `bounceAngle`)
    - Überlange Methoden aufteilen

5. **Unit Tests**
    - Tests für grundlegende Ball-Funktionen wie Bewegung, Kollision, Abprallwinkel usw.

Dieser Ansatz würde den Ball zu einem "dummen" Datencontainer machen, dessen Verhalten durch verschiedene Manager gesteuert wird, was die Wartbarkeit und Erweiterbarkeit verbessert.

## Analyse des Ball-Objekts im Vergleich zum Paddle

Aktuelle Problemstellen im Ball-Objekt
Im Vergleich zum Paddle-Objekt zeigt der Ball mehrere Probleme:

Vermischung von Zuständigkeiten:
Rendering-Code, Physik und Spiellogik sind stark vermischt
Die draw()-Methode enthält auch Spiellogik (Ziel-/Hilfslinie)
Strukturelle Inkonsistenz mit Paddle:

Paddle hat eine saubere Trennung in Rendering-Methoden (drawBase, drawGlueLayer)
Bei Ball ist alles in einer monolithischen draw()-Methode
Abhängigkeiten:
Ball.draw() benötigt eine Paddle-Referenz als Parameter
Direkte Abhängigkeit zu PowerUps (player.powerup[PO_LASER])

## Ball Draw vereinfachen

void Ball::draw(float deltaTime) {
if (eyeCandy)
tail.draw();

    updateGrowth(deltaTime);

    // Logik für das Zeichnen des glued-Balls sollte in eine separate Klasse oder System verlagert werden
    // Hier nur den Ball selbst zeichnen

    glLoadIdentity();
    glTranslatef(pos_x, pos_y, 0.0);
    glColor4f(GL_WHITE);

    if (explosive) {
        fireTex.play(deltaTime);
        // Zeichnen des explosiven Balls...
    } else {
        texture.play(deltaTime);
        // Zeichnen des normalen Balls...
    }
}

// In Ball.h hinzufügen:
void draw() override { draw(0.0f); }
void draw(float deltaTime) override;

## Entkopplung von Player und RuntimeDifficulty
Für die setspeed-Methode:

void Ball::setspeed(GLfloat v) {
// Konstanten verwenden oder als Parameter übergeben
static constexpr GLfloat DEFAULT_MAX_BALL_SPEED = 0.5f;

    // Verwende übergebene Geschwindigkeit oder begrenze auf maximale Geschwindigkeit
    velocity = (v > DEFAULT_MAX_BALL_SPEED) ? DEFAULT_MAX_BALL_SPEED : v;

    getRad();
    xvel = velocity * cos(rad);
    yvel = velocity * sin(rad);
}

## Ball Speed

Die Ballgeschwindigkeit für die verschiedenen Schwierigkeitsgrade wird in mehreren Stellen im Code festgelegt.

Hier sind die wichtigsten Stellen:

1. Die Anfangsgeschwindigkeiten für jede Schwierigkeitsstufe werden in diesen Zeilen definiert:

```cpp
static_difficulty.ballspeed[EASY] = 0.7f;
static_difficulty.ballspeed[NORMAL] = 1.3f;
static_difficulty.ballspeed[HARD] = 1.6f;
```

2. Die maximalen Geschwindigkeiten werden hier festgelegt:

```cpp
static_difficulty.maxballspeed[EASY] = 1.5f;
static_difficulty.maxballspeed[NORMAL] = 2.2f;
static_difficulty.maxballspeed[HARD] = 3.0f;
```

3. Die Geschwindigkeitserhöhung bei Kollision mit Steinen:

```cpp
static_difficulty.hitbrickinc[EASY] = 0.0025;
static_difficulty.hitbrickinc[NORMAL] = 0.003;
static_difficulty.hitbrickinc[HARD] = 0.004;
```

4. Die Geschwindigkeitsverringerung bei Kollision mit dem Paddle:

```cpp
static_difficulty.hitpaddleinc[EASY] = -0.001;
static_difficulty.hitpaddleinc[NORMAL] = -0.0005;
static_difficulty.hitpaddleinc[HARD] = -0.0007;
```

5. Prozentuale Geschwindigkeitserhöhungen:

```cpp
static_difficulty.speedup[EASY] = 10.0f;
static_difficulty.speedup[NORMAL] = 20.0f;
static_difficulty.speedup[HARD] = 30.0f;
```

Die Anwendung dieser Werte erfolgt dann beispielsweise beim Erzeugen eines neuen Balls in der Funktion `bMan.spawn()`, bei Kollisionen in `coldet()` und `padcoldet()`.
