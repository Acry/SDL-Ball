# SettingsManager

Knows the settings

Reads and writes configuration files.
Holds loaded, defaults und current settings.

## Setting types

See: [Settings](../misc/Settings.md)

Grundsätzlich haben wir 3 Typen von Settings:

`defaultSettings` - aus config.h - compileTime settings
`loadedSettings` - gespeicherte Settings des Spielers
`currentSettings` - und die runtime settings

SettingsManager nutzt [ConfigFileManager](ConfigFileManager.md)

## Tasks

Der SettingsManager verwaltet die Spieleinstellungen zentral.

Default-Werte setzen:
Im Konstruktor werden mit setDefaults() die Standardwerte aus config.h geladen und als defaultSettings gespeichert.

Initialisierung:
Die Methode init() lädt die Einstellungen aus der Datei (über ConfigFileManager). Falls das Laden fehlschlägt, werden
die Defaults gesetzt und gespeichert. Nach dem Laden werden die Werte validiert (validateSettings()).

## Load

loadSettings() liest die Einstellungen aus der Datei und speichert sie in loadedSettings.
writeSettings() schreibt die aktuellen Einstellungen (currentSettings) zurück in die Datei.

Änderungserkennung, sollte obsolete sein. emit event settings changed.
CodeManager DisplayManager, SoundManager, etc. können auf Änderungen reagieren.

## Save

hasChanged() prüft, ob sich die aktuellen Einstellungen von den geladenen unterscheiden. Im Destruktor werden geänderte
Einstellungen automatisch gespeichert.

[MenuManager](MenuManager.md) is the only runtime component that can change settings, emits settingChangedFromMenu?

## Validation

`validateSettings()` prüft z.B. die Auflösung und den Schwierigkeitsgrad auf sinnvolle Werte und korrigiert sie ggf.

## Apply

## Kandidaten, die den settingsManager nutzen

- [BackgroundManager](BackgroundManager.md)
- [CodeManager](CodeManager.md)
- [ControllerManager](ControllerManager.md)
- [DisplayManager](DisplayManager.md)
- [HighscoreManager](HighscoreManager.md)
- [LevelManager](LevelManager.md)
- [SaveGameManager](SaveGameManager.md)
- [SceneManager](SceneManager.md)
- [SoundManager](SoundManager.md)
- [TextureManager](TextureManager.md)
- [ThemeManager](ThemeManager.md)

## Future

- add JSON support

TODO: Die settings Struktur könnte in mehrere logische Gruppen aufgeteilt werden:

- Audio
- Video
- Control
- Gameplay
- Themes
- etc.

Player Name?

### First run

Beim ersten Start des Spiels wird die Datei settings.txt nicht gefunden. Der SettingsManager setzt dann die
Standardwerte aus config.h als aktuelle Einstellungen (currentSettings) und speichert sie in der Datei.
Außerdem sett er das Flag `firstRun` auf true, um anzuzeigen, dass es sich um den ersten Start handelt.
Dann wird der Spieler gefragt, ob er das Classic-Theme oder das modern theme nutzen möchte.

Theme Auswahl: Screenshot selectable.
Classic, modern, retro, etc.
