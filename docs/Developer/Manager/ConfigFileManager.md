# ConfigManager

Knows the directories

Der ConfigFileManager verwaltet alle wichtigen Pfade und Verzeichnisse für Konfigurationsdateien, Spielstände,
Highscores und Screenshots deiner Anwendung. Er sorgt dafür, dass diese Verzeichnisse existieren und legt sie bei Bedarf
an. Die Klasse unterstützt dabei sowohl benutzerdefinierte als auch automatisch ermittelte Wurzelverzeichnisse (z.B.
nach [XDG-Standard](https://specifications.freedesktop.org/basedir-spec/latest/) oder im Home-Verzeichnis).

## Archive - no installation required

```text
/sdl-ball-remastered
  /config/
    screenshots/
    savegames/
    highscores/
    /themes
  SDL-Ball-Remastered
```

```bash
make release-archive-zst
make release-archive-zip
```

```makefile
release-archive-zst: THEME_DIRECTORY=./themes/
release-archive-zst: $(BUILD_DIR)$(TARGET)
	$(CXX) $(RELEASE_FLAGS) $(OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)$(TARGET)
```

Currently, ./build/SDL-Ball-remastered/screenshots/ themes/
settings.cfg

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

Currently, it creates the config-folder for [settings.txt](../misc/Settings.md), and the screenshot-folder.

```bash
[carsten@carsten-desktop .config]$ cd sdl-ball/
[carsten@carsten-desktop sdl-ball]$ ls
screenshots settings.cfg
```

`old_main.cpp`

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

## References

[FSH](https://refspecs.linuxfoundation.org/FHS_3.0/fhs/index.html)
[XDG-Docs-Arch](https://wiki.archlinux.org/title/XDG_user_directories)

## Notes - Linux based distros

`$XDG_RUNTIME_DIR`

Konfigurationsdateien:
`$XDG_CONFIG_HOME (meistens ~/.config/)`

Benutzerspezifische Daten:
`$XDG_DATA_HOME (meistens ~/.local/share/)`

Laufzeitdaten (temporär, pro Session):
`$XDG_RUNTIME_DIR (z.\ B. /run/user/$(id -u)/)`

Das sorgt für bessere Integration in moderne Linux-Desktops und vermeidet "Dotfile-Spam" im Home-Verzeichnis.

`$XDG_RUNTIME_DIR` wird im Linux-Umfeld für temporäre, benutzerspezifische Laufzeitdaten verwendet. Typische
Anwendungsfälle sind:

- Speichern von temporären Sockets, Pipes oder Lockfiles, die nur während der aktuellen Benutzersitzung benötigt werden.
- Ablegen von Dateien, die nach einem Neustart oder Logout automatisch gelöscht werden sollen.
- Kommunikation zwischen Prozessen eines Benutzers (z. B. für Desktop-Umgebungen, Daemons oder IPC).

Der Pfad ist nur für den aktuellen Benutzer zugänglich und wird beim Login automatisch angelegt (z. B.
`/run/user/$(id-u)/`).
