# Analyse der EffectManager-Probleme

Der EffectManager hat tatsächlich tiefgreifende Abhängigkeiten zu den Spielobjekten `brick` und `Paddle`, was eine problematische architektonische Kopplung darstellt. Diese Kopplung sollte aufgelöst werden.

## Hauptprobleme

1. **Tiefe Kopplung mit Spielobjekten**: Der EffectManager ruft direkt Methoden von `brick` und `Paddle` auf und hat Zugriff auf deren interne Struktur.

2. **Probleme mit der Variablen `var`/`vars`**: In `EffectManager.h` wird `vars` als globale Variable deklariert, aber in `EffectManager.cpp` wird stattdessen `var.transition_half_done` verwendet.

3. **Bidirektionale Abhängigkeiten**: Die EffectManager-Methoden werden im `main.cpp` für Kollisionserkennung mit verschiedenen Spielobjekten verwendet, was zu komplexen Abhängigkeiten führt.

## Lösungsansatz

### 1. Entkopplung durch Abstrakte Interfaces

Statt direkt mit konkreten Klassen zu arbeiten, sollte der EffectManager mit abstrakten Interfaces arbeiten:

```cpp
// Interface für kollisionsfähige Objekte
class ICollidable {
public:
    virtual bool isActive() const = 0;
    virtual bool checkCollision(float x, float y) const = 0;
    virtual void getCollisionResponse(float x, float y, float& vx, float& vy) const = 0;
    virtual ~ICollidable() = default;
};
```

### 2. Übergabe der Kollisionslogik an den GameManager

Die Kollisionserkennung sollte vom GameManager gesteuert werden, nicht vom EffectManager:

```cpp
// Im GameManager
void updateEffects() {
    // Der GameManager kümmert sich um die Kollisionen
    for (auto& collidable : collidables) {
        if (collidable->isActive()) {
            effectManager.checkCollision(*collidable);
        }
    }
}
```

### 3. Konsistente Verwendung von Variablennamen

Die Variable `var` oder `vars` sollte konsistent verwendet werden. Am besten wäre es, statt einer globalen Variable eine Instanzvariable zu verwenden.

### 4. Zeitmessung modernisieren

Statt auf globale Zeitvariablen zu setzen, sollte die Zeit als Parameter übergeben werden:

```cpp
void EffectManager::update(float deltaTime) {
    for (auto& effect : effects) {
        effect.update(deltaTime);
    }
}

void EffectManager::draw() {
    for (auto& effect : effects) {
        effect.draw();
    }
}
```

## Konkrete Schritte für die Refaktorierung

1. **Interface für Kollidierbare Objekte erstellen**:
    - `ICollidable`-Interface definieren
    - `brick` und `Paddle` so anpassen, dass sie dieses Interface implementieren

2. **EffectManager umstrukturieren**:
    - Kollisionslogik mit dem abstrakten Interface arbeiten lassen
    - Globale Variablen durch Parameter ersetzen
    - Trennung von Update- und Draw-Logik

3. **Konsistente Fehlerbehandlung einführen**:
    - Statt unzuverlässiger Initialisierung und undefinierter Zustände bessere Fehlerbehandlung implementieren

4. **Zeitmanagement verbessern**:
    - Zeitdifferenz als Parameter übergeben statt globaler Variablen
    - Ein zentrales Zeitmanagement einführen

5. **Verantwortlichkeiten klar trennen**:
    - Der GameManager sollte die Spielobjekte und deren Interaktionen verwalten
    - Der EffectManager sollte nur für das Erstellen und Verwalten von Effekten zuständig sein

Die Komplexität dieser Änderungen liegt weniger in der technischen Umsetzung als in der sauberen Trennung der Verantwortlichkeiten und dem Aufbrechen der bidirektionalen Abhängigkeiten.
