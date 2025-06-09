# C++ - OOP

Abstrakte Klassen erkennt man in C++ an mindestens einer pure virtual Methode (mit =0).

## Interfaces - Virtuelle Methoden

Standardargumente (default arguments) können in virtuellen Methoden problematisch sein.

Standardargumente werden zur Compile-Zeit aufgelöst
Virtuelle Methoden werden zur Laufzeit aufgelöst

Überladene nicht-virtuelle Methoden mit Standardargumenten

Best Practices für virtuelle Methoden
Virtuelle Destruktoren

```c++
class Base {
public:
    virtual ~Base() = default;  // Wichtig!
};
```

override-Keyword verwenden

```c++
class Derived : public Base {
    void method() override;  // Compiler-Fehler wenn Base::method nicht existiert
};
```

Pure Virtual vs. Virtual
Pure virtual (=0) für echte Interfaces
Normal virtual für optionales Überschreiben
Default-Implementierung nur wenn sinnvoll

```c++
class Interface {
    virtual void mustImplement() = 0;        // Muss implementiert werden
    virtual void canOverride() { }           // Kann überschrieben werden
};
```

Keine Standardargumente
Stattdessen Überladungen in der Basis-Klasse

final für Endklassen
Verhindert unbeabsichtigte Vererbung
Ermöglicht Compiler-Optimierungen

class Brick final : public GameObject { // Kann nicht weiter vererbt werden
};

Virtual bei allen überschreibbaren Methoden
Wenn eine Methode überschreibbar sein soll, muss sie virtual sein
Sonst: Object Slicing und falsches Verhalten

```c++
// ICollideable.h - Ein "echtes" Interface
class ICollideable {
public:
    virtual ~ICollideable() = default;
    
    // Pure virtual Methoden ohne Standardargumente
    virtual float getPosX() const = 0;
    virtual float getPosY() const = 0;
    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;
    virtual bool isActive() const = 0;
};
```

```c++
// IEventManager.h - Aktuell problematisch
class IEventManager {
public:
    // FALSCH: Pure virtual mit Standardargumenten
    virtual void addListener(GameEvent event, EventCallback callback, void* owner = nullptr) = 0;
    
    // RICHTIG: Pure virtual ohne Standardargumente
    virtual void addListener(GameEvent event, EventCallback callback, void* owner) = 0;
    
    // RICHTIG: Nicht-virtuelle Überladung mit Standardargument
    void addListener(GameEvent event, EventCallback callback) {
        addListener(event, callback, nullptr);
    }
};
```

Die Regeln sind:

Pure virtual (=0) für Interface-Methoden: ✓
Keine Standardargumente in virtuellen Methoden: ✓
Standardargumente stattdessen in nicht-virtuellen Überladungen: ✓

=0 macht eine virtuelle Methode zu einer "pure virtual" Methode. Das bedeutet:

- Die **Methode hat keine Implementierung** in der abstrakten Klasse
- Die **Methode muss in abgeleiteten Klassen implementiert werden**
- Die **Klasse wird dadurch zu einer abstrakten Klasse, die nicht instanziiert werden kann**

## Abstrakte Klassen

Definition einer abstrakten Klasse

```c++
// Abstrakte Basisklasse für Spielobjekte
class GameObject {
public:
    virtual ~GameObject() = default;
    
    // Pure virtuelle Methoden machen die Klasse abstrakt
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(float deltaTime) = 0;
    
    // Normale virtuelle Methoden mit Default-Implementierung
    virtual bool isActive() const { return active; }
    
protected:
    float pos_x{0}, pos_y{0};
    float width{0}, height{0};
    bool active{true};
};
```

Unterschiede zu Interfaces
Abstrakte Klassen können Implementierungen enthalten
Können Membervariablen haben
Können protected/private Members haben
Mindestens eine pure virtuelle Methode

Verwendung

```c++
// Konkrete Klasse erbt von abstrakter Klasse
class Brick final : public GameObject {
public:
    void init() override { }
    void update(float deltaTime) override { }
    void draw(float deltaTime) override;
    
private:
    Color color{};
};
```

## Templates

Templates sind eine wichtige Funktion von C++ für generische Programmierung.

Funktion-Template

```c++
template<typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}
```

Klassen-Template

```c++
template<typename T>
class Container {
private:
    T data;
public:
    explicit Container(T value) : data(value) { }
    T getValue() const { return data; }
    void setValue(T value) { data = value; }
};
```

```c++
// Beispiel für Container-Verwendung
Container<float> pos(0.0f);        // Für Positionen
Container<bool> active(true);      // Für Status
Container<CollisionType> type(CollisionType::Ball);  // Für Enums
```

## Unterschiede zwischen Overload (Überladung) und Override (Überschreiben)

Overload: Gleicher Name, verschiedene Parameter, gleiche Klasse
Override: Gleiche Signatur, virtuelle Basismethode, abgeleitete Klasse

Overload (Überladung):

```c++
class GameObject {
public:
    void setPosition(float x, float y);           // Original
    void setPosition(float x, float y, float z);  // Überladung
    void setPosition(const Vector2D& pos);        // Überladung
};
```

Override (Überschreiben):

```c++
class GameObject {
public:
    virtual void update(float deltaTime) = 0;
};

class Ball : public GameObject {
public:
    void update(float deltaTime) override;  // Überschreibt Basismethode
};
```

## Mehrfachvererbung

Mehrfachvererbung ist in C++ erlaubt, aber kann zu Komplexität führen.

```c++
// Aus Ball.h
class Ball final : public MovingObject, 
                  public GrowableObject, 
                  public ICollideable {
    // ...
};
```

## Komposition vs. Vererbung

Komposition: "Hat ein" Verhältnis, Objekte enthalten andere Objekte

```c++
class BrickManager {
    std::vector<Brick> bricks;           // Komposition
    IEventManager *eventManager;         // Aggregation
    TextureManager *textureManager;      // Aggregation
};
```

## RAII und Ressourcen-Management

```c++
class Ball {
public:
    explicit Ball(EventManager *eventMgr);    // Konstruktor
    ~Ball() override;                         // Destruktor
private:
    EventManager *eventManager;               // Ressource
};
```

## Interface-Segregation (ISP) - "Trennung" oder "Aufteilung"

```c++
// Spezialisierte Interfaces statt einem großen
class ICollideable { /* ... */ };
class IEventManager { /* ... */ };

// Getrennte, spezialisierte Interfaces statt einem großen Interface
class ICollideable {     // Nur Kollisions-bezogene Methoden
    virtual float getPosX() const = 0;
    virtual float getPosY() const = 0;
    // ...
};

class IEventManager {    // Nur Event-bezogene Methoden
    virtual void emit(...) = 0;
    virtual void addListener(...) = 0;
    // ...
};
```
