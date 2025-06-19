# Code-Review

# Code-Review für den SDL-Ball Level Editor

## Allgemeine Beobachtungen:

Der Code implementiert einen einfachen Level-Editor für das SDL-Ball Spiel. Die grundlegende Funktionalität zum
Erstellen, Bearbeiten und Speichern von Levels ist vorhanden.

## Stärken:

- Funktionale Implementation der grundlegenden Editor-Funktionen
- Einfache, verständliche Benutzeroberfläche
- Klare Trennung der Funktionalitäten

## Verbesserungspotentiale:

### 1. Variablendeklaration und Gültigkeitsbereich

- Variablen `bricks`, `i`, `typeSelected` und `powerupSelected` sind global deklariert. Besser wäre es, diese in einer
  Funktion oder einem Objekt zu kapseln.

### 2. Fehlerbehandlung

- Es gibt keine Validierung der Eingabedaten bei `loadData()`
- Keine Fehlerbehandlung, falls die Leveldata ein ungültiges Format hat

### 3. Code-Qualität

- Die `brickGfx`-Funktion könnte durch ein Mapping-Objekt vereinfacht werden:

```javascript
const brickTypeToImage = {
    "0": "blank.png",
    "1": "blue.png",
    // usw.
};
```

- Die Funktion `changeBrick` könnte übersichtlicher strukturiert werden

### 4. Potenzielle Fehlerquellen

- Die `initLevel`-Funktion erstellt ein Array für genau 598 Steine, diese Zahl ist hart codiert
- In der `loadData`-Funktion wird `Data.substr(12,Data.length)` verwendet, diese Magic Number ist fehleranfällig

### 5. Performance und DOM-Manipulation

- Bei jedem Aufruf von `drawLevel()` wird das gesamte Level neu gerendert
- Wiederholte DOM-Manipulationen könnten bei größeren Levels zu Performance-Problemen führen

### 6. Modernes JavaScript

- Es werden `var`-Deklarationen anstelle von `let` oder `const` verwendet
- Es fehlen JSDoc-Kommentare für Funktionen
- Fehlen moderner Array-Methoden wie `map`, `filter`, etc.

## Empfehlungen:

1. Code in ein Modul oder eine Klasse kapseln
2. Globale Variablen vermeiden
3. `let` und `const` anstelle von `var` verwenden
4. Bessere Fehlerbehandlung implementieren
5. Magic Numbers durch benannte Konstanten ersetzen
6. JSDoc-Kommentare für Funktionen hinzufügen
7. Event-Listener statt Inline-JavaScript verwenden
8. Selektive DOM-Updates anstatt komplettes Neu-Rendern

Mit diesen Änderungen wäre der Code besser wartbar, robuster und würde modernen JavaScript-Standards entsprechen.