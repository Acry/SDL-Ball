# Brick-Concept

## Brick Types

enum class BrickType {
None,
Blue,
Yellow,
Cement,
Glass,
Green,
Grey,
Purple,
White,
Invisible,
Red,
Explosive,
Doom,
Base
};

int BrickManager::getBaseScore(BrickType type) {
switch(type) {
case BrickType::Cement:    return 300;
case BrickType::Glass:     return 200;
case BrickType::Invisible: return 250;
case BrickType::Explosive: return 400;
default:                   return 100;
}
}
Bei Typ '3' (Zementsteine): Der Score wird auf 300 gesetzt, wenn der Stein "breakable" wird.
player.score += score * player.multiply * var.averageBallSpeed; //Speed bonus
see score

Normale Bricks: Standard-Blöcke (Typ '1')
Diese geben normale Punktzahl und erfordern einen Treffer zum Zerstören

Zement-Bricks (Typ '3')
Unzerstörbar, bis sie durch breakable() in zerstörbare Blöcke umgewandelt werden
Nach Umwandlung: 300 Punkte und 1 benötigter Treffer

Glas-Bricks (Typ '4')
Werden mit breakable() in zerstörbare Blöcke umgewandelt

Unsichtbare Bricks (Typ '9')
Ähnlich wie Glas-Bricks, aber zuerst unsichtbar
Erscheinen nach dem ersten Treffer

Explosive Bricks (Typ 'B')
Lösen eine Kettenreaktion aus und zerstören benachbarte Blöcke
Werden durch Funktion makeExplosive() umgewandelt

Doom-Bricks (Typ 'C')
Spezielle Blöcke mit eigenem Soundeffekt

Farbige Bricks
Aus dem Texture-Loading-Code erkennbar: blau, gelb, grün, grau, lila, weiß und rot
Diese haben vermutlich ähnliche Eigenschaften, aber unterschiedliches Aussehen
Anzahl der benötigten Treffer (hitsLeft)
Verhalten beim Treffen (normales Verschwinden oder Explosion)
Visuellen Effekten und Soundeffekten
Die hit()-Methode ist der zentrale Punkt, an dem die Punkte vergeben werden:

growExplosive: Lässt benachbarte Bricks explosiv werden
breakable: Macht einen Brick zerstörbar (durch Powerup)

Power-up "Explosive Grow" lässt explosive Bricks wachsen
Power-up "Easy Brick" macht alle Bricks leichter zu zerstören
Power-up "Detonate" sprengt alle explosiven Bricks
Power-up "Drop" lässt alle Bricks nach unten fallen

## BrickManager

[BrickManage](../Manager/BrickManager.md)

## BrickFade

Sollte ich shrinking und fading kombinieren?
colliding, but active

// In Brick.h
class Brick : public GameObject {
private:
bool active = true;
bool visible = true;
bool destroyed = false;
bool fading = false;
float fadeValue = 1.0f;
static constexpr float FADE_SPEED = 0.05f; // Anpassbar nach Bedarf

public:
// Bestehende Methoden...

    void setDestroyed() {
        destroyed = true;
        fading = true;
        // Kollision deaktivieren, aber weiterhin sichtbar und aktiv lassen
    }
    
    void update(float deltaTime) {
        // Bestehende Update-Logik
        
        // Fade-Effekt aktualisieren
        if (fading) {
            fadeValue -= FADE_SPEED * deltaTime * 60.0f; // Zeit-normalisiert
            if (fadeValue <= 0.0f) {
                fadeValue = 0.0f;
                fading = false;
                active = false; // Deaktiviere den Brick vollständig
            }
            // Aktualisiere die Transparenz für das Rendering
            setAlpha(fadeValue);
        }
    }
    
    [[nodiscard]] bool isDestroyed() const { return destroyed; }
    [[nodiscard]] bool isFading() const { return fading; }
    [[nodiscard]] bool isActive() const { return active; }
    [[nodiscard]] bool isVisible() const { return visible; }
    [[nodiscard]] bool isCollisionEnabled() const { return active && !destroyed; }

};

## Brick Shrink

## Brick Explosion

```c++
// Aktualisiere die Brick-Klasse, damit sie von GrowableObject erbt
class Brick : public GrowableObject {
public:
// Konstruktor
Brick() {
// Standardwerte setzen
growSpeed = 0.5f; // Anpassbar je nach gewünschter Geschwindigkeit
keepAspectRatio = true;
aspectRatio = 0.5f; // Breite:Höhe Verhältnis, anpassbar
}

    // Implementierung der onSizeChanged-Methode
    void onSizeChanged() override {
        // Aktualisiere die Position basierend auf dem Zentrum
        pos_x = centerX - width / 2.0f;
        pos_y = centerY - height / 2.0f;
        
        // Wenn notwendig, aktualisiere Kollisionsboxen oder andere größenabhängige Eigenschaften
    }
    
    // Methode zum Starten des Verschwindens eines Bricks
    void fadeOut() {
        // Setze ein kleines Ziel für die Größe, was den Effekt des Verschwindens erzeugt
        setGrowTarget(0.01f);
    }
    
    // Override der draw-Methode, um die Größenanpassung zu berücksichtigen
    void draw() {
        if (active && (width > 0.01f)) {
            // Zeichenlogik mit aktueller Größe
            // ...
            
            // Wenn die Größe sehr klein ist und wir schrumpfen, deaktiviere den Brick
            if (width <= 0.02f && shrinking) {
                active = false;
                // Andere Aufräumarbeiten...
            }
        }
    }
};

```

## Explosion

Explosionsanimation und Explosionsradius im alten Code
Im gezeigten Code gibt es mehrere Elemente, die mit der Explosion von Blöcken zu tun haben:

Explosionstypen und -auslöser:

Ein Block hat ein type Attribut, wobei 'B' für explosive Blöcke steht
Die Methode detonateExplosives() wird aufgerufen, um alle explosiven Blöcke zu zünden
Die hit() Methode wird bei Explosionen mit unterschiedlichen Parametern aufgerufen
Explosionseffekte:

Der Effektmanager fxMan wird verwendet, um visuelle Effekte zu erzeugen
Positionsinformationen p und Geschwindigkeitsinformationen v werden übergeben
Explosionsmechanik:

Es gibt ein Powerup PO_EXPLOSIVE_GROW, das gewöhnliche Blöcke in explosive Blöcke verwandelt
Die explosiveGrow() Funktion lässt explosive Eigenschaften auf benachbarte Blöcke übergreifen
Die Funktion growExplosive() wird für einzelne Blöcke aufgerufen
Besonders interessant für deinen Zweck ist die explosiveGrow() Funktion, die zeigt, wie der Explosionsradius
funktioniert:

```c++
void explosiveGrow(brick bricks[])
{
  for(int i=0; i < 598; i++)
  {
    if(bricks[i].active)
    {
      bricks[i].growExplosive(bricks);
    }
  }

  for(int i=0; i < 598; i++)
  {
    bricks[i].justBecomeExplosive=0;
  }
}
```

Ich verstehe jetzt, dass du die Explosionsanimation und den Explosionsradius aus dem alten Code für dein aktuelles
Projekt übernehmen möchtest. Hier findest du die relevanten Teile, die du benötigst:

### Explosionsradius im alten Code

Die `growExplosive`-Methode im alten Code definiert den Explosionsradius. Sie prüft alle 8 umgebenden Ziegel (
horizontal, vertikal und diagonal):

```cpp
void growExplosive(brick bricks[]) const {
    if (type != 'B' || justBecomeExplosive) {
        return;
    }
    // Prüft alle 8 umliegenden Positionen und macht benachbarte Bricks explosiv
    // Links, rechts, oben, unten und die 4 diagonalen Richtungen
    // ...
}
```

### Explosionsanimation im alten Code

Die Explosionsanimation besteht aus mehreren Teilen:

1. **Partikeleffekte**: In der `hit`-Methode werden bei explosiven Bricks spezielle Partikeleffekte erzeugt:

```cpp
// Partikeleffekte für explosive Bricks
p.x = pos_x;
p.y = pos_y;
fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
fxMan.set(FX_VAR_COLDET, 1);
fxMan.set(FX_VAR_LIFE, 1200);
fxMan.set(FX_VAR_NUM, 10);
fxMan.set(FX_VAR_SIZE, 0.08f);
fxMan.set(FX_VAR_SPEED, 0.4f);
fxMan.set(FX_VAR_GRAVITY, -1.3f);

fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f); // Orange-gelbe Farben
fxMan.spawn(p);
// ...weitere Partikel mit leicht abweichenden Farben
```

2. **Kettenreaktion**: In der `draw`-Methode wird die Kettenreaktion implementiert:

```cpp
if (isexploding && !var.paused) {
    fade -= 7.0f * globalMilliTicksSinceLastDraw;
    opacity = fade;
    if (fade < 0.0f) {
        active = false;

        // Umliegende Bricks treffen, um Kettenreaktion auszulösen
        position spos, svel;
        spos.x = pos_x;
        spos.y = pos_y;
        
        // Prüft alle 8 umliegenden Positionen und löst hit() aus
        // ...
    }
}
```

### Integration in dein aktuelles System

Für dein aktuelles System könntest du `GrowableObject` mit dem Explosionsmechanismus kombinieren:

```cpp
class ExplosiveBrick : public Brick, public GrowableObject {
private:
    bool isExploding = false;
    float explosionTimer = 0.0f;
    static constexpr float EXPLOSION_DURATION = 0.5f;
    
public:
    ExplosiveBrick() {
        // GrowableObject Eigenschaften setzen
        growSpeed = 1.0f;
        keepAspectRatio = true;
    }
    
    void startExplosion() {
        isExploding = true;
        explosionTimer = EXPLOSION_DURATION;
        
        // Schrumpfen starten (könnte auch aufblähen für Explosion)
        setGrowTarget(0.01f);
        
        // Partikeleffekte hier auslösen
        // (Du brauchst einen Partikel-Manager in deinem neuen System)
        
        // Umgebende Bricks in die Explosion einbeziehen
        detonateNeighbors();
    }
    
    void update(float deltaTime) override {
        Brick::update(deltaTime);
        
        if (isExploding) {
            explosionTimer -= deltaTime;
            
            // GrowableObject aktualisieren
            updateGrowth(deltaTime);
            
            if (explosionTimer <= 0.0f) {
                setActive(false);
                isExploding = false;
            }
        }
    }
    
    void onSizeChanged() override {
        // Position aktualisieren basierend auf dem Mittelpunkt
        pos_x = centerX - width / 2.0f;
        pos_y = centerY - height / 2.0f;
    }
    
    void detonateNeighbors() {
        // Ähnlich wie im alten Code, angepasst an dein neues BrickManager-System
        // Du müsstest hier über den BrickManager Zugriff auf Nachbarn bekommen
    }
};
```

### BrickManager-Erweiterung

Im BrickManager müsstest du die Explosionslogik implementieren:

```cpp
void BrickManager::detonateExplosiveBrick(size_t index) {
    // Position des explodierenden Bricks speichern
    float centerX = bricks[index].pos_x + bricks[index].width / 2.0f;
    float centerY = bricks[index].pos_y + bricks[index].height / 2.0f;
    
    // Alle umgebenden Bricks finden und detonieren
    // Dies ist ähnlich wie die alte growExplosive-Methode
    // Du musst eine Möglichkeit finden, Bricks in einem bestimmten Radius zu identifizieren
    
    constexpr float EXPLOSION_RADIUS = BRICK_WIDTH * 1.5f;
    
    for (size_t i = 0; i < bricks.size(); i++) {
        if (i == index || !bricks[i].isActive()) continue;
        
        // Mittelpunkt des anderen Bricks
        float otherCenterX = bricks[i].pos_x + bricks[i].width / 2.0f;
        float otherCenterY = bricks[i].pos_y + bricks[i].height / 2.0f;
        
        // Abstand zwischen den Brick-Mittelpunkten berechnen
        float dx = centerX - otherCenterX;
        float dy = centerY - otherCenterY;
        float distance = std::sqrt(dx*dx + dy*dy);
        
        // Wenn der Brick im Explosionsradius liegt
        if (distance <= EXPLOSION_RADIUS) {
            // Simuliere einen Treffer am betroffenen Brick
            CollisionData collData;
            collData.object2 = &bricks[i];
            onBallHitBrick(collData);
        }
    }
    
    // Partikeleffekte für die Explosion
    // Hier müsstest du dein Partikelsystem einbinden
}
```

Und dann im `onBallHitBrick`:

```cpp
case BrickType::Explosive:
    if (health <= 0) {
        detonateExplosiveBrick(index);
        // Emit explosion event x, y für Sound und andere Effekte
        EventData explosionData;
        explosionData.sender = &brick;
        explosionData.posX = brick.pos_x + brick.width / 2.0f;
        explosionData.posY = brick.pos_y + brick.height / 2.0f;
        eventManager->emit(GameEvent::ExplosionOccurred, explosionData);
    }
    break;
```

Diese Implementierung kombiniert das Schrumpfen der Bricks über `GrowableObject` mit dem Explosionsmechanismus aus dem
alten Code, angepasst an dein neues Event-System. Du müsstest noch ein Partikelsystem hinzufügen, um die visuellen
Effekte der Explosion zu reproduzieren.
