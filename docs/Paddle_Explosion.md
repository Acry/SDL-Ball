# Paddle-Explosionseffekt implementieren

Der Code für eine Paddle-Explosion fehlt noch. Eine Explosionsanimation ist wichtig, wenn das Paddle durch ein Game-Over-Ereignis zerstört wird.

Hier ist meine Implementierungsvorschlag:

## 1. Erweiterung von `Paddle.h`

Füge diese Eigenschaften und Methoden zur `Paddle`-Klasse hinzu:

```cpp
// In Paddle.h im öffentlichen Bereich
bool isExploding;
float explosionDuration;
float explosionTimer;
int explosionParticles;

void explode();
void updateExplosion(float deltaTime);
void drawExplosion();
```

## 2. Implementation in `Paddle.cpp`

Ändere die `init()`-Methode:

```cpp
void Paddle::init() {
    growing = false;
    growspeed = 0.05f;
    aspect = 0.2f;
    pos_y = -0.93f;
    pos_x = 0.0f;
    width = 0.059f;
    height = 0.018f;
    dead = false;
    isExploding = false;
    explosionDuration = 1.0f;
    explosionTimer = 0.0f;
    explosionParticles = 15;
    // Initialisierung der Layer-Flags
    hasGlueLayer = false;
    hasGunLayer = false;
}
```

Explode-Methode hinzufügen:

```cpp
void Paddle::explode() {
    if (!dead && !isExploding) {
        isExploding = true;
        explosionTimer = 0.0f;
    }
}
```

Explosionsaktualisierung erweitern:

```cpp
void Paddle::updateExplosion(float deltaTime) {
    if (isExploding) {
        explosionTimer += deltaTime;
        
        if (explosionTimer >= explosionDuration) {
            isExploding = false;
            dead = true;
        }
    }
}
```

Die `update()`-Methode anpassen:

```cpp
void Paddle::update(float deltaTime) {
    if (isExploding) {
        updateExplosion(deltaTime);
    } else {
        // Normalupdate nur wenn nicht explodierend
        updateGrowth(deltaTime);
    }
}
```

Eine Explosionszeichnungsmethode implementieren:

```cpp
void Paddle::drawExplosion() {
    // Explosionspartikel
    float progress = explosionTimer / explosionDuration;
    float size = width * (1.0f - progress) * 0.2f;
    float spread = width * 2.0f * progress;
    
    glDisable(GL_TEXTURE_2D);
    
    for (int i = 0; i < explosionParticles; i++) {
        // Zufällige Positionen für die Partikel
        float angle = (i * 360.0f / explosionParticles) * 0.0174533f; // In Radian umwandeln
        float distance = spread * (0.2f + 0.8f * ((i % 3) / 2.0f));
        float x = pos_x + cos(angle) * distance;
        float y = pos_y + sin(angle) * distance * 1.2f; // Leicht ovales Explosionsmuster
        
        // Farbe - von weiß zu rot/orange verblassend
        float alpha = 1.0f - progress;
        glColor4f(1.0f, 1.0f - progress * 0.8f, 0.2f - progress * 0.2f, alpha);
        
        // Partikel zeichnen
        glLoadIdentity();
        glTranslatef(x, y, 0.0f);
        glBegin(GL_QUADS);
        glVertex3f(-size, size, 0.0f);
        glVertex3f(size, size, 0.0f);
        glVertex3f(size, -size, 0.0f);
        glVertex3f(-size, -size, 0.0f);
        glEnd();
    }
}
```

Die `draw()`-Methode erweitern:

```cpp
void Paddle::draw() {
    if (dead) return;
    
    if (isExploding) {
        drawExplosion();
    } else {
        drawBase();

        if (hasGlueLayer)
            drawGlueLayer();

        if (hasGunLayer)
            drawGunLayer();
    }
}
```

## 3. Verwendung im Spielcode

```cpp
// Beispiel zur Auslösung der Explosion
if (spielerHatVerloren) {
    paddle.explode();
}
```

Diese Implementierung bietet:
- Eine animierte Explosion mit Partikeln
- Dynamische Farb- und Größenänderung
- Automatische Markierung des Paddles als "tot" nach der Explosion
- Unterbrechung der normalen Paddle-Funktionalität während der Explosion

Die Partikel verteilen sich in einem leicht ovalen Muster und verblassen von weiß zu rot/orange, während sie sich vom ursprünglichen Paddle-Standort wegbewegen.