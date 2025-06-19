# ThemeManager

Die ThemeManager-Klasse ist für die Verwaltung von Spielthemen (Grafiken, Sounds, Level) zuständig.

Hier sind die Hauptkomponenten:

Datenstrukturen
themeInfo: Struktur zur Speicherung von Theme-Metadaten
name: Name des Themes
snd, gfx, lvl: Flags für vorhandene Ressourcentypen
valid: Theme enthält mindestens einen Ressourcentyp

Haupt-Attribute
themes: Liste aller verfügbaren Themes
currentTheme: Aktuell ausgewähltes Theme
configFileManager: Referenz für Dateisystempfade
pathCache: Cache für bereits aufgelöste Ressourcenpfade

Kernfunktionalitäten
Theme-Erkennung (scanThemes):

Durchsucht User- und globale Theme-Verzeichnisse
Identifiziert Themes und deren Inhaltstypen (gfx, snd, lvl)
Speichert Metadaten für jedes Theme
Theme-Verwaltung:

listThemes(): Gibt Liste verfügbarer Themes zurück
themeExists(): Prüft, ob ein Theme existiert
setCurrentTheme(): Setzt aktives Theme
getCurrentTheme(): Gibt aktives Theme zurück
Ressourcen-Zugriff (getThemeFilePath):

Findet Dateien nach Theme-Hierarchie:
Nutzer-Theme-Verzeichnis
Globales Theme-Verzeichnis
Default-Theme (Fallback)
Nutzt Caching für schnelleren Zugriff
Gibt vollständigen Dateipfad oder leeren String zurück

Hilfsfunktionen:
getDefaultTheme(): Liefert Name des Standard-Themes
themeHasResource(): Prüft, ob eine Ressource in einem Theme existiert

Die Klasse arbeitet eng mit dem ConfigFileManager zusammen und bietet eine zentrale Schnittstelle für das Laden von themenspezifischen Ressourcen im gesamten Spiel.

## Responsibilities

Handles the themes, loads the theme file and applies it to the game

## Theming

The game should run without any theme

Themes:
- Fonts
- gfx
- sound
- Levels
- Backgrounds

Should have a default theme
When theme is another theme-folder, it can be incrementally loaded and apply only the changes

Backgrounds should be handled separately

UI (HUD)
UI (Menu)

GameObjects

Effects

## Theme Pathes

Get from [ConfigFileManager](ConfigFileManager.md)
Order should be:
from private to system

## Die Rolle von ThemeManager.cpp

Der `ThemeManager` ist eine zentrale Komponente für die Themenverwaltung im Projekt und hat folgende Hauptfunktionen:

1. **Themen-Erkennung:**
    - Durchsucht Verzeichnisse nach Themen-Ordnern
    - Prüft auf erforderliche Unterverzeichnisse und Dateien (gfx, snd, levels.txt)
    - Erfasst verfügbare Themen und deren Eigenschaften

2. **Themen-Verwaltung:**
    - Bietet Zugriff auf die Liste verfügbarer Themen
    - Stellt Methoden zur Überprüfung der Existenz von Themen bereit
    - Definiert ein Standardthema als Fallback

3. **Dateipfad-Resolution:**
    - Ermittelt korrekte Dateipfade basierend auf Thema und Ressourcentyp
    - Implementiert eine Hierarchie zur Suche (nutzerspezifisch → global → default)
    - Behandelt Fehler, wenn Dateien nicht gefunden werden

Der `ThemeManager` arbeitet eng mit dem `ConfigFileManager` zusammen, um die korrekten Verzeichnisse für Themen zu finden, und bietet eine zentrale Schnittstelle für das Laden von themenspezifischen Ressourcen.

In der Todo-Liste gibt es einen Eintrag "rethink the role of the ThemeManager", was darauf hindeutet, dass seine Funktion möglicherweise überarbeitet werden sollte, ähnlich wie bei der Beobachtung zur Umbenennung von `Texture` zu `TextureAnimation`.


## Vergleich der Theme-Verwaltung

Ich sehe mir den älteren Quellcode im Vergleich zum neuen ThemeManager an:

Im ursprünglichen Code war die Theme-Verwaltung verteilt auf mehrere einfache Komponenten:

1. **Structure und Settings**:
   ```cpp
   struct themeInfo {
     string name;
     bool snd,gfx,lvl,valid;
   };
   
   struct settings {
     string sndTheme,gfxTheme,lvlTheme;
     // weitere Einstellungen
   }
   ```

2. **Funktionen**:
   - `useTheme(string path, string theme)` - Versucht eine Datei in verschiedenen Theme-Verzeichnissen zu finden
   - `getThemes()` - Durchsucht Verzeichnisse nach Themes und gibt Informationen zurück

3. **Standardeinstellungen** in `main()`:
   ```cpp
   setting.gfxTheme="default";
   setting.sndTheme="default";
   setting.lvlTheme="default";
   ```

Die neue ThemeManager-Klasse, die wir entworfen haben, bietet folgende Verbesserungen:

1. **Zentralisierte Verwaltung** statt verstreuter Funktionen
2. **Erweiterte Funktionalität** wie Theme-Hierarchie und Vererbung
3. **Effiziente Ressourcennutzung** durch Caching
4. **Verbesserte Fehlerbehandlung**
5. **Bessere Struktur** durch eine dedizierte Klasse

Die grundlegende Funktionalität ist ähnlich, aber der neue ThemeManager ist flexibler und bietet eine bessere Codeorganisation mit mehr Funktionen.
