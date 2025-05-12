# Schwierigkeitsgrade

`Config.h` - CompileTime Config
`ConfigFileSettings.h` - Runtime Config

Der Schwierigkeitsgrad wird aktuell in der Datei `config.h` definiert.

Es gibt drei verschiedene Schwierigkeitsgrade: `EASY`, `NORMAL` und `HARD`.

```c++
// Game Difficulties
#define EASY   0
#define NORMAL 1
#define HARD   2

#define DEFAULT_DIFFICULTY    EASY
```

Default ist EASY.

Der [SettingsManager](SettingsManager.md) ist dafür zuständig, die Einstellungen zu laden und zu speichern.
Er lädt den schieriegkeitsgrad, falls vorhanden, sonst liefert er den defaultwert zurück.

Grundsätzlich haben wir 3 Arten von Werten:

- default - Werte kommen aus `config.h`
- loaded - aus dem SettingsManager
- current - Werden durch das Spiel oder den Spieler spelbst angepasst.

Der SettingsManager liefert also zu Beginn den Schwierigkeitsgrad.

Der Schwierikeitsgrad sieht wie folgt aus:

```c++
struct difficulty {
    GLfloat ballspeed[3];
    GLfloat maxballspeed[3];
    GLfloat hitbrickinc[3];
    GLfloat hitpaddleinc[3];
    GLfloat slowdown[3];
    GLfloat speedup[3];
    Uint32 coins[3];
    Uint32 life[3];
};
```

Diese werden in der `difficultyStruct` gespeichert. Der Schwierigkeitsgrad wird in der `player` Struktur gespeichert und kann mit der Funktion `setDifficulty()` geändert werden. Die Funktion `getDifficulty()` gibt den aktuellen Schwierigkeitsgrad zurück.
