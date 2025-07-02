# SettingsManager

Der SettingsManager kapselt das Laden, Validieren, Verwalten und Speichern aller relevanten Spieleinstellungen und sorgt
dafür, dass immer konsistente Werte verwendet werden.

See: [Settings](../misc/Settings.md)

- default
- loaded
- current

Grundsätzlich haben wir 3 Typen von Settings:

`default` - aus config.h - compileTime settings
`loaded` - gespeicherte Settings des Spielers
`current` - und die runtime settings

Hält loaded, defaults und current settings.

SettingsManager nutzt [ConfigFileManager](ConfigFileManager.md)

## Kernaufgaben

Der SettingsManager verwaltet die Spieleinstellungen zentral.

Er übernimmt folgende Aufgaben:

Default-Werte setzen:
Im Konstruktor werden mit setDefaults() die Standardwerte aus config.h geladen und als defaultSettings gespeichert.

Initialisierung:
Die Methode init() lädt die Einstellungen aus der Datei (über ConfigFileManager). Falls das Laden fehlschlägt, werden
die Defaults gesetzt und gespeichert. Nach dem Laden werden die Werte validiert (validateSettings()).

Laden und Speichern:

loadSettings() liest die Einstellungen aus der Datei und speichert sie in loadedSettings.
writeSettings() schreibt die aktuellen Einstellungen (currentSettings) zurück in die Datei.

Validierung:
validateSettings() prüft z.B. die Auflösung und den Schwierigkeitsgrad auf sinnvolle Werte und korrigiert sie ggf.

Zustandsverwaltung:
Es gibt drei Settings-Strukturen:

defaultSettings: Kompilierte Standardwerte
loadedSettings: Aus Datei geladene Werte
currentSettings: Aktuelle Werte zur Laufzeit (können verändert werden)

Änderungserkennung:
hasChanged() prüft, ob sich die aktuellen Einstellungen von den geladenen unterscheiden. Im Destruktor werden geänderte
Einstellungen automatisch gespeichert.

Getter/Setter:
Es gibt Methoden, um z.B. den Schwierigkeitsgrad oder den Fullscreen-Modus abzufragen oder zu setzen.

## Kandidaten, die den settingsManager nutzen

- BackgroundManager
- CodeManager
- ControllerManager
- DisplayManager
- HighscoreManager
- LevelManager
- SaveGameManager
- SceneManager
- SoundManager
- TextureManager
- ThemeManager

## Future

- add JSON support

TODO: Die settings Struktur könnte in mehrere logische Gruppen aufgeteilt werden:

- Audio
- Video
- Steuerung
- Gameplay
