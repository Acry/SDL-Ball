# Tracer

Auch wenn der Tracer ein Anhängsel des Balls ist, wäre es sauberer, wenn er ebenfalls von MovingObject und eventuell sogar von GrowableObject erben würde. Das würde zu konsistenterem Code und besserer Wartbarkeit führen.

```c++
class Tracer : public MovingObject, public GrowableObject {
private:
struct Particle {
GLfloat x, y;           // Position
GLfloat r, g, b, a;     // Farbe und Alpha
GLfloat scale;          // Größe/Skalierung
bool active;            // Ist das Partikel aktiv?
};

    std::vector<Particle> particles;
    GLfloat cr, cg, cb;         // Aktuelle Farben
    int color;
    GLfloat lastX, lastY;       // Letzte Position
    bool isActive;              // Gesamtstatus des Tracers

public:
SpriteSheetAnimation tex;

    Tracer(int particleCount = 100);
    
    // Getter/Setter für active-Status
    void setActive(bool active) { isActive = active; }
    [[nodiscard]] bool getActive() const { return isActive; }
    
    // Implementation der virtuellen Getter/Setter aus GrowableObject
    [[nodiscard]] GLfloat getWidth() const override { return MovingObject::width; }
    [[nodiscard]] GLfloat getHeight() const override { return MovingObject::height; }
    void setWidth(GLfloat w) override { MovingObject::width = w; }
    void setHeight(GLfloat h) override { MovingObject::height = h; }
    
    void draw(float deltaTime) override;
    void update(float deltaTime) override;
    
    // Neue Methode für Positionsaktualisierung
    void updatePosition(GLfloat nx, GLfloat ny);
    
    void colorRotate(bool explosive, const GLfloat c[]);
    
    // Neue Reset-Methode
    void reset();
};
```

## Überschneidungen mit dem EffectManager-System

Es gibt klare Überschneidungen zwischen den Tracer-Funktionalitäten und dem EffectManager-System.

```c++
class Tracer : public MovingObject, public GrowableObject {
private:
    int effectId{-1};         // ID des aktuellen Effekts im EffectManager
    EffectManager& effectMgr; // Referenz zum EffectManager
    glm::vec2 lastPosition{0.0f};
    bool isActive{true};
    float scale{1.0f};        // Größenskalierung

public:
    // Konstruktor mit Dependency Injection für den EffectManager
    explicit Tracer(EffectManager& manager) : effectMgr(manager) {}

    void setActive(bool active) {
        isActive = active;
        
        // Wenn deaktiviert, Effekt im Manager killen
        if (!isActive && effectId >= 0) {
            effectMgr.kill(effectId);
            effectId = -1;
        }
    }
    
    [[nodiscard]] bool getActive() const { return isActive; }

    // MovingObject & GrowableObject Implementierungen
    [[nodiscard]] GLfloat getWidth() const override { return width; }
    [[nodiscard]] GLfloat getHeight() const override { return height; }
    void setWidth(GLfloat w) override { width = w; scale = w / 10.0f; }
    void setHeight(GLfloat h) override { height = h; }

    // Keine eigene draw-Methode nötig, wird vom EffectManager gehandhabt

    void update(float deltaTime) override {
        // Keine Implementierung nötig, der EffectManager aktualisiert die Partikel
    }

    void updatePosition(GLfloat nx, GLfloat ny) {
        if (!isActive) return;

        glm::vec2 currentPos(nx, ny);
        position p{nx, ny, 0.0f};

        // Wenn wir noch keinen Effekt haben, einen erstellen
        if (effectId < 0 || !effectMgr.isActive(effectId)) {
            // Effekt konfigurieren
            effectMgr.set(FX_VAR_TYPE, FX_SPARKS);
            effectMgr.set(FX_VAR_NUM, 20);           // Anzahl der Partikel
            effectMgr.set(FX_VAR_SPEED, 0.0f);       // Keine Eigenbewegung
            effectMgr.set(FX_VAR_SPREAD, 360.0f);    // Volle Streuung
            effectMgr.set(FX_VAR_SIZE, scale);       // Größe basierend auf Ball
            effectMgr.set(FX_VAR_LIFE, 800);         // Lebensdauer in ms
            effectMgr.set(FX_VAR_GRAVITY, 0.0f);     // Keine Gravitation
            effectMgr.set(FX_VAR_COLDET, false);     // Keine Kollision
            effectMgr.set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f); // Weiße Grundfarbe
            
            // Effekt erzeugen
            effectId = effectMgr.spawn(p);
        } else {
            // Bei existierendem Effekt können wir ein Partikelfeld verwenden
            // und die Position aktualisieren
            position newPos{nx, ny, 0.0f};
            
            // Alternativ einen neuen Funken am aktuellen Ort erzeugen
            // wenn genug Bewegung stattgefunden hat
            if (glm::distance(currentPos, lastPosition) > 0.01f) {
                effectMgr.spawn(p);
            }
        }

        lastPosition = currentPos;
    }

    void setColor(GLfloat r, GLfloat g, GLfloat b) {
        // Farbe für neue Partikel setzen
        effectMgr.set(FX_VAR_COLOR, r, g, b);
    }

    void reset() {
        // Alten Effekt beenden
        if (effectId >= 0) {
            effectMgr.kill(effectId);
            effectId = -1;
        }

        // Bei Bedarf neuen Effekt starten, wenn aktiv
        if (isActive) {
            // Wird bei nächstem updatePosition erstellt
        }
    }
};
```

## Der Tracer synchronisiert sich nicht mit dem Status des Balls.

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

## modernere Lösung

```c++
class Tracer : public MovingObject, public GrowableObject {
private:
    class Particle {
    public:
        glm::vec2 position;     // Position als 2D-Vektor
        glm::vec4 color;        // RGBA als 4D-Vektor
        float scale;            // Größe/Skalierung
        float lifetime;         // Aktuelle Lebensdauer
        float maxLifetime;      // Maximale Lebensdauer
        float velocity;         // Bewegungsgeschwindigkeit
        glm::vec2 direction;    // Bewegungsrichtung als normalisierter Vektor
        bool active;            // Ist das Partikel aktiv?

        Particle() : position(0.0f), color(1.0f), scale(1.0f), 
                    lifetime(0.0f), maxLifetime(1.0f), velocity(0.0f),
                    direction(0.0f, 0.0f), active(false) {}
                    
        void update(float deltaTime) {
            if (!active) return;
            
            lifetime += deltaTime;
            if (lifetime >= maxLifetime) {
                active = false;
                return;
            }
            
            // Farbe und Größe basierend auf Lebensdauer anpassen
            color.a = 1.0f - (lifetime / maxLifetime);
            scale *= 0.99f;  // Langsam schrumpfen
            
            // Position aktualisieren
            position += direction * velocity * deltaTime;
        }
    };

    std::vector<Particle> particles;
    std::size_t nextParticleIndex{0};
    glm::vec3 currentColor{1.0f};
    glm::vec2 lastPosition{0.0f};
    bool isActive{true};
    
    // Partikel-Pool-Management
    Particle& getNextParticle() {
        // Suche zuerst inaktive Partikel
        for (std::size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].active) {
                return particles[i];
            }
        }
        
        // Wenn alle aktiv sind, überschreibe das älteste
        Particle& result = particles[nextParticleIndex];
        nextParticleIndex = (nextParticleIndex + 1) % particles.size();
        return result;
    }

public:
    SpriteSheetAnimation tex;

    Tracer(int particleCount = 100) : particles(particleCount) {}

    void setActive(bool active) { isActive = active; }
    [[nodiscard]] bool getActive() const { return isActive; }

    // MovingObject & GrowableObject Implementierungen
    [[nodiscard]] GLfloat getWidth() const override { return width; }
    [[nodiscard]] GLfloat getHeight() const override { return height; }
    void setWidth(GLfloat w) override { width = w; }
    void setHeight(GLfloat h) override { height = h; }

    void draw(float deltaTime) override {
        if (!isActive) return;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (auto& particle : particles) {
            if (!particle.active) continue;
            
            // Texturen und Positionen setzen
            glPushMatrix();
            glTranslatef(particle.position.x, particle.position.y, 0.0f);
            glScalef(particle.scale, particle.scale, 1.0f);
            
            // Farbe setzen
            glColor4f(particle.color.r, particle.color.g, 
                     particle.color.b, particle.color.a);
            
            // Textur zeichnen
            tex.draw();
            
            glPopMatrix();
        }
        
        glDisable(GL_BLEND);
    }
    
    void update(float deltaTime) override {
        if (!isActive) return;
        
        // Alle aktiven Partikel aktualisieren
        for (auto& particle : particles) {
            if (particle.active) {
                particle.update(deltaTime);
            }
        }
    }

    void updatePosition(GLfloat nx, GLfloat ny) {
        if (!isActive) return;
        
        glm::vec2 currentPos(nx, ny);
        glm::vec2 direction = currentPos - lastPosition;
        
        if (glm::length(direction) > 0.001f) {
            // Nur neue Partikel erstellen, wenn signifikante Bewegung
            Particle& p = getNextParticle();
            p.active = true;
            p.position = currentPos;
            p.color = glm::vec4(currentColor, 1.0f);
            p.scale = 0.05f * width;  // Abhängig von Ballgröße
            p.lifetime = 0.0f;
            p.maxLifetime = 0.8f;
            p.velocity = 0.0f;  // Stationäre Partikel für Tracer
            p.direction = glm::normalize(direction);
        }
        
        lastPosition = currentPos;
    }

    void colorRotate(bool explosive, const GLfloat c[]) {
        // Farbrotation für normale und explosive Partikel
        if (explosive) {
            currentColor = glm::vec3(1.0f, 0.3f + 0.7f * c[0], 0.0f);
        } else {
            currentColor = glm::vec3(c[0], c[1], c[2]);
        }
    }

    void reset() {
        for (auto& p : particles) {
            p.active = false;
        }
        nextParticleIndex = 0;
    }
};
```

## Mögliche Integration in die Ball-Klasse

```cpp
// In Ball.h
class Ball : public MovingObject, public GrowableObject {
private:
    Tracer tracer;
    bool eyeCandy;
    // Weitere Ball-Eigenschaften...
    
public:
    explicit Ball(EffectManager& effectManager) : tracer(effectManager), eyeCandy(true) {
        // Ball-Initialisierung
    }
    
    void update(const float deltaTime) override {
        // Wenn Ball inaktiv, auch Tracer deaktivieren
        if (!active) {
            tracer.setActive(false);
            return;
        }
        
        // Tracer-Aktivität an eyeCandy-Einstellung koppeln
        tracer.setActive(eyeCandy);
        
        // Bestehender Ball-Code...
        
        // Tracer-Position aktualisieren wenn Ball aktiv und bewegt sich
        if (eyeCandy && (xvel != 0.0f || yvel != 0.0f)) {
            tracer.updatePosition(pos_x, pos_y);
        }
    }
    
    void reset() {
        active = true;
        // Ball-Reset-Code...
        
        // Tracer zurücksetzen
        tracer.reset();
        tracer.setActive(eyeCandy);
    }
};
```