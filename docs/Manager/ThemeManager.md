# ThemeManager

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
