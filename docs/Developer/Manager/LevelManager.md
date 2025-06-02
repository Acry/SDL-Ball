# LevelManager

Einige bricks laufen in späteren rechts aus dem Bild.

Der LevelManager parst das ThemeFile und stellt die Level-Informationen bereit.
getLevel().

get_level_data
read_levels_structure

set_up_bricks_for_level - soll der das machen?
load_levels() war ein anderer Ansatz


- [levels](../misc/level.md)
- [level_format](../misc/level_format.md)

See: [Managers](../Managers.md)

## Umstrukturierung

## Analyse der LevelManager-Umstrukturierung

Basierend auf dem bereitgestellten Code und der bisherigen Konversation sieht dein Ansatz zur Umwandlung des
LevelManager-Namespaces in eine Klasse sinnvoll aus, mit klaren Verantwortlichkeitsbereichen.

### Aktuelle Struktur

- **LevelManager.h**: Du hast bereits eine Klassenstruktur definiert
- **LevelManager.cpp**: Enthält noch globale Funktionen und Variablen
- Es gibt noch parallele Strukturen (z.B. `powerupLoaderClass` vs. `PowerupLoader`)

### Empfehlungen zur Implementierung

1. **Verantwortlichkeiten trennen**:
    - Der LevelManager sollte nur Level-Daten laden und bereitstellen
    - Die Funktion `set_up_bricks_for_level` gehört in den BrickManager
    - PowerupLoader könnte entweder Teil des PowerupManagers werden oder als Hilfsobjekt dienen

2. **LevelManager-Implementierung**:
   ```cpp
   bool LevelManager::readLevelsStructure(const std::string &filename) {
       // Bisherige Implementierung von read_levels_structure
       level_ranges.clear(); // Jetzt Member-Variable
       // Rest der Funktion
   }

   bool LevelManager::getLevelData(size_t level, brick bricks[], const std::string &filename) {
       // Bisherige Implementierung von get_level_data
       if (level >= level_ranges.size()) {
           // ...
       }
       // Rest der Funktion
   }

   bool LevelManager::loadLevels() {
       const std::string levels_file_name = themeManager.getThemeFilePath("levels.txt", setting.lvlTheme);
       return readLevelsStructure(levels_file_name);
   }
   ```

3. **BrickManager-Schnittstelle**:
    - Der BrickManager sollte eine Methode haben, die Level-Daten vom LevelManager erhält
    - Er übernimmt dann die Positionierung und Texturierung der Bricks

4. **PowerupManager-Integration**:
    - Der PowerupManager sollte Informationen über Powerups in Bricks vom BrickManager erhalten
    - Er muss nicht direkt mit dem LevelManager interagieren

### Schnittstellen zwischen den Managern

```cpp
// BrickManager erhält Level-Daten vom LevelManager
bool BrickManager::setupLevel(size_t level) {
    brick tempBricks[MAX_BRICKS];
    if (!levelManager.getLevelData(level, tempBricks, themeManager.getThemeFilePath("levels.txt", setting.lvlTheme))) {
        return false;
    }
    
    // Bricks positionieren und texturieren (ehemals set_up_bricks_for_level)
    // ...
    
    // Powerup-Information an PowerupManager übergeben
    for (int i = 0; i < MAX_BRICKS; i++) {
        if (tempBricks[i].powerup != '0') {
            powerupManager.registerPowerupBrick(i, tempBricks[i].powerup);
        }
    }
    
    return true;
}
```

Diese Struktur erlaubt saubere Trennung der Verantwortlichkeiten und entspricht einem modernen, objekt-orientierten
Design. Für den LevelManager-Test kannst du ein vereinfachtes Rendering implementieren, ohne BrickManager zu verwenden.

## Level-Editor

See: [index.htlm](../../../Website/sdl-ball.sourceforge.net/leveleditor/index.html)

## Old Code

Im alten Code werden die Positionen der Steine (bricks) in der Funktion initlevels wie folgt berechnet:

Daraus ergeben sich die Grenzen für das Spielfeld der Steine:

Linke Grenze: -1.54 (Startwert für posx)
Obere Grenze: 0.95 (Startwert für posy)
Rechte Grenze:
Es gibt 26 Steine pro Zeile (brick von 0 bis 25).
Rechte Grenze: -1.54 + 0.1232 * 25 = 1.526 (letzter Stein)
Untere Grenze:
Es gibt 23 Reihen (row von 0 bis 22).
Untere Grenze: 0.95 - 0.07 * 22 = -0.59 (letzte Reihe)

Zusammenfassung:

- Links: -1.54
- Rechts: 1.526
- Oben: 0.95
- Unten: -0.59

Diese Werte definieren das Spielfeld für die Steine.
