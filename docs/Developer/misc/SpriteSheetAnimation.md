# Die Rolle von Texture.cpp im Projekt

Die Klasse `Texture` und die zugehörige Implementierung in `Texture.cpp` spielen eine zentrale Rolle im Texturmanagement-System des Projekts.
Die Klasse verwaltet insbesondere animierte Texturen mit folgenden Hauptfunktionen:

- Die Texture-Klasse ist primär für die Animation von Sprite-Sheets zuständig
- Die Hauptmethode play() kümmert sich um Frame-Wechsel und Texturkoordinaten-Berechnung
- Die Klasse verwaltet Animationszustände (Richtung, aktueller Frame, Timing)

1. **Verwaltung von Animationszuständen:**
    - Verfolgt den aktuellen Frame einer Animation
    - Handhabt die Aktualisierung der Frames basierend auf Zeitintervallen (Ticks)
    - Unterstützt bidirektionale Animationen (vorwärts und rückwärts)

2. **Berechnung von Texturkoordinaten:**
    - Berechnet die korrekten UV-Koordinaten für jeden Frame einer Spritesheet-Textur
    - Unterstützt Padding zwischen einzelnen Frames
    - Organisiert Frames in einem Raster (Zeilen und Spalten)

3. **Animation und Wiedergabe:**
    - Die `play()`-Methode ist das Herzstück der Klasse
    - Aktualisiert den aktuellen Frame basierend auf verstrichener Zeit
    - Berechnet die Texturkoordinaten für den aktuellen Frame

Die Klasse arbeitet eng mit `TextureManager` zusammen, der für das Laden und die grundlegende Verwaltung von Texturen zuständig ist, während `Texture` selbst für die Animation und die Berechnung der korrekten Texturkoordinaten beim Rendern verantwortlich ist.

Die `textureProperties`-Struktur enthält alle notwendigen Informationen über die Textur, wie Größe, Layout des Spritesheets, Animationsgeschwindigkeit und mehr.