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