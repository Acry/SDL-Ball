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
Currently, it creates the config-folder for [settings.txt](../misc/Settings.md), and the screenshot-folder.

___

```bash
[carsten@carsten-desktop .config]$ cd sdl-ball/
[carsten@carsten-desktop sdl-ball]$ ls
highscores.txt savegame.sav screenshots settings.cfg themes
```

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

## Notes

$XDG_RUNTIME_DIR

Konfigurationsdateien:
$XDG_CONFIG_HOME (meistens ~/.config/)

Benutzerspezifische Daten:
$XDG_DATA_HOME (meistens ~/.local/share/)

Laufzeitdaten (temporär, pro Session):
$XDG_RUNTIME_DIR (z.\ B. /run/user/$(id -u)/)

Das sorgt für bessere Integration in moderne Linux-Desktops und vermeidet "Dotfile-Spam" im Home-Verzeichnis.

$XDG_RUNTIME_DIR wird im Linux-Umfeld für temporäre, benutzerspezifische Laufzeitdaten verwendet. Typische
Anwendungsfälle sind:

- Speichern von temporären Sockets, Pipes oder Lockfiles, die nur während der aktuellen Benutzersitzung benötigt werden.
- Ablegen von Dateien, die nach einem Neustart oder Logout automatisch gelöscht werden sollen.
- Kommunikation zwischen Prozessen eines Benutzers (z. B. für Desktop-Umgebungen, Daemons oder IPC).

Der Pfad ist nur für den aktuellen Benutzer zugänglich und wird beim Login automatisch angelegt (z. B. /run/user/$(id
-u)/).
