Ja, diese `main.cpp` ist sehr unübersichtlich. Hier sind die Hauptprobleme und Verbesserungsvorschläge:

1. Größe und Verantwortlichkeiten:
```cpp
// Aufteilen in mehrere Module:
core/
  Application.cpp/hpp    // SDL/OpenGL Setup, Main-Loop
  GameLoop.cpp/hpp       // Spiellogik
  InputHandler.cpp/hpp   // Event-Handling
  RenderManager.cpp/hpp  // Rendering
```

2. Globale Variablen und Strukturen:
```cpp
class GameState {
private:
    static GameState* instance;
    settings gameSettings;
    player_struct currentPlayer;
    gameVars vars;
    // etc.
};
```

3. Resourcen-Management:
```cpp
class ResourceManager {
private:
    std::map<std::string, textureClass> textures;
    std::map<std::string, GLuint> displayLists;
public:
    void loadTextures();
    void cleanupResources();
};
```

4. Main-Loop Struktur:
```cpp
class Application {
private:
    std::unique_ptr<GameLoop> gameLoop;
    std::unique_ptr<InputHandler> input;
    std::unique_ptr<RenderManager> renderer;
    
public:
    void run() {
        while (!shouldQuit) {
            input->handleEvents();
            gameLoop->update();
            renderer->render();
        }
    }
};
```

5. Konfiguration:
```cpp
class Config {
private:
    static Config* instance;
    std::string configPath;
    
public:
    void loadFromFile();
    void saveToFile();
};
```

Hauptvorteile der Refaktorierung:
1. Bessere Trennung der Zuständigkeiten
2. Vermeidung globaler Variablen
3. Bessere Testbarkeit
4. Einfachere Wartung
5. Klarere Struktur
6. Wiederverwendbare Komponenten

Implementiere die Änderungen schrittweise, um die Stabilität zu gewährleisten.