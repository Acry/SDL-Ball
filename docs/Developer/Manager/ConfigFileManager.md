# ConfigManager

Der ConfigFileManager verwaltet alle wichtigen Pfade und Verzeichnisse für Konfigurationsdateien, Spielstände,
Highscores und Screenshots deiner Anwendung. Er sorgt dafür, dass diese Verzeichnisse existieren und legt sie bei Bedarf
an. Die Klasse unterstützt dabei sowohl benutzerdefinierte als auch automatisch ermittelte Wurzelverzeichnisse (z.B.
nach XDG-Standard oder im Home-Verzeichnis).

## Kernaufgaben

- Ermittelt beim Start das Wurzelverzeichnis für Konfigurationsdaten (XDG, HOME oder lokal).
- Erstellt das Spiel-Unterverzeichnis darin.
- Setzt und verwaltet die Pfade für Einstellungen, Spielstände, Highscores und Screenshots.
- Legt das Screenshot-Verzeichnis explizit an und weicht bei Problemen auf Alternativen aus.
- Stellt Methoden bereit, um die Verzeichnisse zu prüfen und ggf. zu erstellen (checkDir).
- Bietet Getter für alle relevanten Pfade und Theme-Verzeichnisse.

## Beispiel:

Beim Erstellen eines ConfigFileManager mit leerem Pfad sucht er automatisch das passende Verzeichnis, legt es an und
initialisiert alle Pfade. Die Methoden wie getSettingsFile() oder getScreenshotDir() liefern dann die fertigen Pfade
zurück.

___

Knows the places

Reads and writes configuration files.
Currently, it creates the config (settings.txt) and the screenshot-folder.

___

[carsten@carsten-desktop .config]$ cd sdl-ball/
[carsten@carsten-desktop sdl-ball]$ ls
highscores.txt savegame.sav screenshots settings.cfg themes

old_main.cpp

```c++
#ifndef DATADIR
  #define DATADIR "themes/"
#endif

```

Makefile

```Makefile
# Arch Linux
PREFIX?=/usr/

# Arch Linux
DATADIR?=$(PREFIX)share/sdl-ball/themes/
```
