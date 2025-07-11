# ThemeManager

Knows the themes

## Overview

Handles the themes, loads the theme file and applies it to the game.

Die ThemeManager-Klasse ist für die Verwaltung von Spielthemen (Grafiken, Sounds, Level) zuständig.

Nun könnte der codemanager den eventmanager und alle nötigen manager laden.
dann kann der thememanager prüfen welche themes verfügbar und welches gewünscht ist.
er ermittiert das event ThemeRequested, // ThemeManager: Theme requested event

Die Listener können dann auf das Event reagieren und die entsprechenden Ressourcen laden.
Wenn der ThemeManager rücklüfer hat, sendet er das Event:

ThemeChanged, // ThemeManager: Theme changed event

Hier sind die Hauptkomponenten:

## Details

### Datenstrukturen

themeInfo: Struktur zur Speicherung von Theme-Metadaten

- name: Name des Themes
- snd, gfx, lvl: Flags für vorhandene Ressourcentypen
- valid: Theme enthält mindestens einen Ressourcentyp

### Haupt-Attribute

themes: Liste aller verfügbaren Themes
currentTheme: Aktuell ausgewähltes Theme
configFileManager: Referenz für Dateisystempfade
pathCache: Cache für bereits aufgelöste Ressourcenpfade

### Kernfunktionalitäten

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

Die Klasse arbeitet eng mit dem ConfigFileManager zusammen und bietet eine zentrale Schnittstelle für das Laden von
themenspezifischen Ressourcen im gesamten Spiel.

### Theming

Current SubThemes:

- Backgrounds
- Fonts
- Levels
- Music
- Sound
- Sprites

Should have a default theme
When theme is another theme-folder, it can be incrementally loaded and apply only the changes

Backgrounds should be handled separately? no -> incrementally

???
UI (HUD)
UI (Menu)
GameObjects
Effects

### Theme Paths

Get from [ConfigFileManager](ConfigFileManager.md)

Themes can be stored in this place in that order:

`~/.config/sdl-ball-remastered/themes/` - user dir
or the installation dir of the included themes
`~/.local/share/sdl-ball-remastered/themes/` - local dir
`/usr/share/sdl-ball-remastered/themes/` -global dir

The ThemeManager will walk trought these directories and look for themes.

### Theme Names

What about theme unique names?

## Die Rolle von ThemeManager.cpp -> Overview

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

## Theming -> docs

Instructions to build a theme: ?page=themes
