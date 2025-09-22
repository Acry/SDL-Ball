# Structure

```text
code-manager
main
  audio
  config
  core-game
  display
  effects
  events
  input
  menu
  scenes
  settings
```

CodeManager: Initialisiert alle Kernkomponenten (Settings, Ressourcen, Event-System) und bereitet alles für den
Szenenwechsel vor.
SceneManager: Steuert, welche Szene gerade aktiv ist (Titel, Credits, Autoplay, Spiel). Er übernimmt die Kontrolle nach
der Initialisierung durch den CodeManager.
GameManager: Wird vom SceneManager in der Spielszene verwendet und verwaltet den eigentlichen Spielablauf (Level, Logik,
Interaktion oder Autoplay).

Der EventManager ist das zentrale Bindeglied für die Ereignissteuerung.
Er verarbeitet alle Events (z.B. Eingaben, System- oder Spielereignisse) und benachrichtigt die registrierten
Komponenten (Listener).
Dadurch sind die einzelnen Manager- und Presenter-Klassen entkoppelt und reagieren nur auf relevante Events, was die
Architektur flexibel und erweiterbar macht.
Der EventManager läuft permanent im Hintergrund und sorgt für die Kommunikation zwischen den Modulen.

## Class types

- Manager
- Presenter
- Scene
- GameObject
- Drawer

A presenter class combines different drawing-classes.

