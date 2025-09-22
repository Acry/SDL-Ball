Du kannst abstrakte Game States als `enum class` und eine zentrale State\-Verwaltung einführen\.  
Dadurch weiß das Spiel jederzeit, ob du im Menü, in einer Szene oder im eigentlichen Spiel bist\.  
Die Logik für Input und Rendering kann dann je nach State unterschiedlich reagieren\.  
Beispiel:

```cpp
// GameState.h
#pragma once

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    TitleScene,
    Credits,
    GameOver,
    Settings,
    Shop
};
```

```cpp
// GameStateManager.h
#pragma once
#include "GameState.h"

class GameStateManager {
    GameState currentState{GameState::MainMenu};

public:
    GameState getState() const { return currentState; }
    void setState(GameState state) { currentState = state; }
    bool isPlaying() const { return currentState == GameState::Playing; }
    // Weitere Hilfsmethoden je nach Bedarf
};
```

Im Haupt\-Loop und bei Event\-Handling prüfst du dann den aktuellen State und reagierst entsprechend\.

Die Game States sollten direkt beim Input\-Handling berücksichtigt werden\.  
Das bedeutet:  
\- Im Event\-Dispatcher oder Input\-Controller prüfst du vor der Verarbeitung eines Inputs den aktuellen GameState\.  
\- Je nach State werden die Events an unterschiedliche Manager weitergeleitet oder ignoriert \(z\.B\. im Menü keine
Ball\-Aktionen\)\.

**Beispiel:**

```cpp
// Im EventDispatcher oder InputController
void handleInputEvent(const InputEvent &event) {
    switch (gameStateManager.getState()) {
        case GameState::MainMenu:
            // Nur Menü\-Navigation zulassen
            break;
        case GameState::Playing:
            // Input an BallManager, PaddleManager etc\. weiterleiten
            break;
        case GameState::Paused:
            // Nur Pause\-Menü\-Aktionen
            break;
        // Weitere States
        default:
            break;
    }
}
```

So wird Input kontextsensitiv verarbeitet und die Spiellogik bleibt sauber getrennt\.

## Architektur

Um die Architektur sauber zu erweitern und GameStates/Scenes einzubinden, solltest du folgende Schichten und
Verantwortlichkeiten klar trennen:

1\. **CodeManager**  
\- Zentrale Steuerung des Spiels  
\- Initialisiert und hält Referenzen auf alle Manager (SceneManager, EventDispatcher, InputManager, GameManager, etc.)  
\- Startet den Hauptloop und delegiert an SceneManager

2\. **SceneManager**  
\- Verwalten der aktuellen Scene/GameState (`MainMenu`, `Playing`, `Paused`, etc.)  
\- Wechselt zwischen Scenes und hält den aktuellen State  
\- Stellt sicher, dass nur die zur Scene passenden Manager/Logik aktiv sind

3\. **EventDispatcher/InputManager**  
\- Verarbeitet Input und Events  
\- Fragt beim SceneManager den aktuellen State ab  
\- Leitet Events nur an die relevanten Manager weiter (z\.B\. im Menü keine Ball\-Aktionen)

**Beispiel für Zusammenspiel:**

```cpp
// CodeManager.cpp
void CodeManager::mainLoop() {
    while (running) {
        sceneManager->update();
        eventDispatcher->processEvents(sceneManager->getCurrentState());
        sceneManager->render();
    }
}
```

```cpp
// EventDispatcher.cpp
void EventDispatcher::processEvents(GameState state) {
    // Input nur an passende Manager weiterleiten
    switch (state) {
        case GameState::MainMenu:
            // Nur Menü-Events
            break;
        case GameState::Playing:
            // Game-Events (BallManager, PaddleManager, etc.)
            break;
        // ...
    }
}
```

**Vorteil:**  
\- Klare Trennung von Steuerung, Scene\-Logik und Event\-Verarbeitung  
\- Erweiterbar um neue States/Scenes  
\- Testbar und wartbar

So bleibt die Architektur modular und flexibel.
