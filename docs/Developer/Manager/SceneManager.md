# SceneManager

Angenommen ich habe eine Szene Titel. Ein 3D Logo mit Sternenhintergund auf dem ein feuerball der durch den bildschirm
bounced.

Nach ein paar Sekunden, wird an die Game Scene übergeben, der GameManager bekommt den param: autoplay true, spielt ein
paar sekunden. Dann geht es zur Scene highscore, auf der werden die highscores schön präsentiert. dann wieder autoplay,
dann eine szene mit game-credits - you get the idea.

[Scenes](../Scenes/Intro.md) for a list of all scenes.

## Todo

SceneManager is responsible for managing the different scenes in the game.
Der ThemeManager muss aufgebohrt werden. snd (Audio, Graphics, Level, Fonts, Powerups, Backgrounds, Title (Scenes?)).

- [ ] add a new scene for the used keys
- [ ] add a new scene for more explanation, see Website page=about.html
- [ ] autoplay after title for a while
- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] credits scene
- [ ] TitleScreen, rework
  Blocker GameManager, SceneManager, MenuManager
- [ ] TitleScreen_Tests
- [ ] SceneManager create
- [ ] SceneManager_Tests
- [ ] add a pause indicator / Pause Scene
- [ ] show current level no# during gameplay, IDK get ready for level X, fade in paddle and ball, play sounds

## Sketch

#ifndef SCENE_H
#define SCENE_H

class Scene {
public:
virtual ~Scene() = default;
virtual void initialize() = 0; // Called when scene is first loaded
virtual void update(float deltaTime) = 0; // Update logic
virtual void render() = 0; // Render the scene
virtual void cleanup() = 0; // Called when scene is unloaded
};

    void addScene(const std::string& name, std::unique_ptr<Scene> scene) {
        scenes[name] = std::move(scene);
    }

    // Set the active scene by name
    void setActiveScene(const std::string& name) {
        auto it = scenes.find(name);
        if (it != scenes.end()) {
            if (activeScene) {
                activeScene->cleanup(); // Clean up previous scene
            }
            activeScene = it->second.get();
            activeScene->initialize(); // Initialize new scene
        }
    }

    // Update the active scene
    void update(float deltaTime) {
        if (activeScene) {
            activeScene->update(deltaTime);
        }
    }

    // Remove a scene by name
    void removeScene(const std::string& name) {
        scenes.erase(name);
    }

    // Clear all scenes
    void clearScenes() {
        scenes.clear();
        activeScene = nullptr;
    }

    // Get the current active scene's name
    std::string getActiveSceneName() const {
        for (const auto& pair : scenes) {
            if (pair.second.get() == activeScene) {
                return pair.first;
            }
        }
        return "";
    }

private:
std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
Scene* activeScene; // Pointer to the currently active scene
};

Add a TransitionType currentTransition member to SceneManager.
In setActiveScene, set the desired transition type (e.g., randomly or based on settings from SettingsManager).
In render, switch on currentTransition to select the appropriate shader and rendering logic:

void render() {
activeScene->render(incomingFbo->fbo);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = transitionTime / transitionDuration;
    switch (currentTransition) {
        case TransitionType::SLIDE_HORIZONTAL:
            renderSlide(t);
            break;
        case TransitionType::FADE:
            renderFade(t);
            break;
        // Add cases for other transitions
    }

}

## Shaping the sketch

virtual void render(GLuint targetFBO) = 0; // Jede Szene rendert in ein FBO

std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
Scene* currentScene = nullptr;
Scene* nextScene = nullptr;
std::string currentSceneName;
std::string nextSceneName;

std::chrono::steady_clock::time_point sceneStartTime;

    // FBOs für Übergänge
    struct SceneFBO {
        GLuint fbo = 0;
        GLuint texId = 0;
        int width = 0;
        int height = 0;
    };

    SceneFBO currentFBO;
    SceneFBO nextFBO;

    void initializeFBOs(int width, int height) {
        // FBOs für aktuelle und nächste Szene erstellen
        createFBO(currentFBO, width, height);
        createFBO(nextFBO, width, height);
    }

    void createFBO(SceneFBO& fbo, int width, int height) {
        fbo.width = width;
        fbo.height = height;
        
        // Textur für FBO erstellen
        glGenTextures(1, &fbo.texId);
        glBindTexture(GL_TEXTURE_2D, fbo.texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // FBO erstellen und Textur zuweisen
        glGenFramebuffers(1, &fbo.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texId, 0);
        
        // Prüfen ob FBO vollständig ist
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO creation failed");
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

public:

    SceneManager(int width, int height, EffectFactory* effectFactory)
        : m_effectFactory(effectFactory) {
        initializeFBOs(width, height);
    }
    
    ~SceneManager() {
        // FBOs aufräumen
        glDeleteFramebuffers(1, &currentFBO.fbo);
        glDeleteFramebuffers(1, &nextFBO.fbo);
        glDeleteTextures(1, &currentFBO.texId);
        glDeleteTextures(1, &nextFBO.texId);
    }
    
    void registerScene(const std::string& name, std::unique_ptr<Scene> scene) {
        scenes[name] = std::move(scene);
    }
    
    void changeScene(const std::string& name, TransitionEffectType transitionType) {
        auto it = scenes.find(name);
        if (it != scenes.end()) {
            nextSceneName = name;
            nextScene = it->second.get();
            
            // Übergang initialisieren
            currentTransition = m_effectFactory->createTransitionEffect(transitionType);
            if (currentTransition) {
                currentTransition->activate();
                transitioning = true;
                
                // Szenen in FBOs rendern
                if (currentScene) {
                    glBindFramebuffer(GL_FRAMEBUFFER, currentFBO.fbo);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    currentScene->render(currentFBO.fbo);
                }
                
                glBindFramebuffer(GL_FRAMEBUFFER, nextFBO.fbo);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                nextScene->render(nextFBO.fbo);
                
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                
                // Texturen an den Übergangseffekt übergeben
                currentTransition->setTextures(currentFBO.texId, nextFBO.texId);
            } else {
                // Sofortiger Wechsel ohne Übergang
                completeSceneChange();
            }
        }
    }
    
    void update(float deltaTime) {
        if (transitioning) {
            currentTransition->update(deltaTime);
            if (!currentTransition->isActive()) {
                completeSceneChange();
            }
        } else if (currentScene) {
            currentScene->update(deltaTime);
            
            // Auto-Transition basierend auf Szenendauer
            int duration = currentScene->getDuration();
            if (duration > 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - sceneStartTime).count();
                
                if (elapsed >= duration) {
                    // Wechsel zur nächsten Szene in der Liste
                    auto it = scenes.find(currentSceneName);
                    if (it != scenes.end()) {
                        auto nextIt = std::next(it);
                        if (nextIt == scenes.end()) nextIt = scenes.begin();
                        changeScene(nextIt->first, TransitionEffectType::CrossFade);
                    }
                }
            }
        }
    }
    
    void render() {
        if (transitioning) {
            // Nur den Übergangseffekt rendern, der die beiden FBO-Texturen verwendet
            currentTransition->draw();
        } else if (currentScene) {
            // Direkt zum Bildschirm rendern
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            currentScene->render(0); // 0 = Default Framebuffer
        }
    }

private:
EffectFactory* m_effectFactory;

    void completeSceneChange() {
        if (currentScene) {
            currentScene->cleanup();
        }
        currentScene = nextScene;
        currentSceneName = nextSceneName;
        nextScene = nullptr;
        
        if (currentScene) {
            currentScene->initialize();
            sceneStartTime = std::chrono::steady_clock::now();
        }
        
        transitioning = false;
    }

};

Wichtige Punkte:

FBO-Verwaltung: Der SceneManager erstellt und verwaltet die FBOs für beide Szenen. Bei einem Übergang werden beide
Szenen in ihre jeweiligen FBOs gerendert und dann für den Übergangseffekt verwendet.

Klare Zuständigkeiten:
InputManager/EventDispatcher - Eingabebehandlung
Scene - Logik und Rendering der eigentlichen Szene
SceneManager - Koordination von Szenenübergängen und FBO-Verwaltung
TransitionEffect - Animation des Übergangs zwischen den Szenen
Szenendauer: Es gibt eine getDuration-Methode, die einen automatischen Wechsel nach einer bestimmten Zeit ermöglicht.

Diese Architektur sollte die komplexe Interaktion zwischen Szenen, Übergängen und Rendering-Targets deutlich besser
abbilden als mein vorheriger Vorschlag.

## Final Thoughts ?

Ja, ich verstehe deine Idee vollständig. Du möchtest einen komplexen Szenenablauf mit unterschiedlichen interaktiven
Szenen, die automatisch wechseln und Übergänge haben.

Hier ist eine verbesserte Implementierung des SceneManagers, die genau diesen Anforderungen entspricht:

```cpp
// Scene.h
class Scene {
public:
    virtual ~Scene() = default;
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(GLuint targetFBO) = 0;
    virtual void cleanup() = 0;
    
    // Gibt die Dauer der Szene in Millisekunden zurück
    // -1 bedeutet manueller Wechsel (kein Auto-Wechsel)
    virtual int getDuration() const { return -1; }
    
    // Bestimmt die nächste Szene (optional überschreiben)
    virtual std::string getNextSceneName() const { return ""; }
    
    // Parameter für die nächste Szene (optional überschreiben)
    virtual std::map<std::string, std::string> getNextSceneParams() const { 
        return {}; 
    }
};
```

```cpp
// SceneManager.h
class SceneManager {
private:
    struct SceneFBO {
        GLuint fbo = 0;
        GLuint texId = 0;
        int width = 0;
        int height = 0;
    };

    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
    std::string currentSceneName;
    std::string nextSceneName;
    std::map<std::string, std::string> nextSceneParams;

    std::unique_ptr<TransitionEffect> currentTransition;
    bool transitioning = false;
    std::chrono::steady_clock::time_point sceneStartTime;

    SceneFBO currentFBO;
    SceneFBO nextFBO;
    EffectFactory* m_effectFactory;
    GameManager* m_gameManager;  // Referenz auf den GameManager

public:
    SceneManager(int width, int height, EffectFactory* effectFactory, GameManager* gameManager)
        : m_effectFactory(effectFactory), m_gameManager(gameManager) {
        initializeFBOs(width, height);
    }

    ~SceneManager() {
        // FBOs aufräumen
        cleanupFBOs();
    }

    void registerScene(const std::string& name, std::unique_ptr<Scene> scene) {
        scenes[name] = std::move(scene);
    }

    void changeScene(const std::string& name, 
                     TransitionEffectType transitionType,
                     const std::map<std::string, std::string>& params = {}) {
        
        auto it = scenes.find(name);
        if (it == scenes.end()) return;
        
        nextSceneName = name;
        nextScene = it->second.get();
        nextSceneParams = params;
        
        // Übergangseffekt erstellen
        switch (transitionType) {
            case TransitionEffectType::FadeIn:
                currentTransition = m_effectFactory->createFadeInEffect();
                break;
            case TransitionEffectType::FadeOut:
                currentTransition = m_effectFactory->createFadeOutEffect();
                break;
            case TransitionEffectType::CrossFade:
                currentTransition = m_effectFactory->createCrossFadeEffect();
                break;
            // usw. für andere Übergangseffekte
        }
        
        if (currentTransition) {
            currentTransition->activate();
            transitioning = true;
            
            // Szenen in FBOs rendern
            renderScenesToFBOs();
            
            // Texturen an den Übergangseffekt übergeben
            currentTransition->setTextures(currentFBO.texId, nextFBO.texId);
        } else {
            // Sofortiger Wechsel ohne Übergang
            completeSceneChange();
        }
    }

    void update(float deltaTime) {
        if (transitioning) {
            currentTransition->update(deltaTime);
            if (!currentTransition->isActive()) {
                completeSceneChange();
            }
        } else if (currentScene) {
            currentScene->update(deltaTime);
            
            // Auto-Transition basierend auf Szenendauer
            int duration = currentScene->getDuration();
            if (duration > 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - sceneStartTime).count();
                
                if (elapsed >= duration) {
                    // Nächste Szene von der aktuellen Szene bestimmen
                    std::string nextScene = currentScene->getNextSceneName();
                    if (nextScene.empty()) {
                        // Wenn keine spezifische nächste Szene angegeben, 
                        // nimm die nächste in der Liste
                        auto it = scenes.find(currentSceneName);
                        if (it != scenes.end()) {
                            auto nextIt = std::next(it);
                            if (nextIt == scenes.end()) nextIt = scenes.begin();
                            nextScene = nextIt->first;
                        }
                    }
                    
                    if (!nextScene.empty()) {
                        // Parameter für die nächste Szene übernehmen
                        auto params = currentScene->getNextSceneParams();
                        changeScene(nextScene, TransitionEffectType::CrossFade, params);
                    }
                }
            }
        }
    }

    void render() {
        if (transitioning) {
            // Nur den Übergangseffekt rendern
            currentTransition->draw();
        } else if (currentScene) {
            // Direkt zum Bildschirm rendern
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            currentScene->render(0);
        }
    }

private:
    void initializeFBOs(int width, int height) {
        createFBO(currentFBO, width, height);
        createFBO(nextFBO, width, height);
    }
    
    void createFBO(SceneFBO& fbo, int width, int height) {
        // FBO und Textur erstellen wie in deinem Entwurf
        // ...
    }
    
    void cleanupFBOs() {
        glDeleteFramebuffers(1, &currentFBO.fbo);
        glDeleteFramebuffers(1, &nextFBO.fbo);
        glDeleteTextures(1, &currentFBO.texId);
        glDeleteTextures(1, &nextFBO.texId);
    }
    
    void renderScenesToFBOs() {
        // Aktuelle Szene in currentFBO rendern
        if (currentScene) {
            glBindFramebuffer(GL_FRAMEBUFFER, currentFBO.fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            currentScene->render(currentFBO.fbo);
        }
        
        // Nächste Szene in nextFBO rendern
        if (nextScene) {
            glBindFramebuffer(GL_FRAMEBUFFER, nextFBO.fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            nextScene->render(nextFBO.fbo);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void completeSceneChange() {
        if (currentScene) {
            currentScene->cleanup();
        }
        
        currentScene = nextScene;
        currentSceneName = nextSceneName;
        nextScene = nullptr;
        
        // Parameter an den GameManager übergeben (falls nötig)
        if (m_gameManager && nextSceneParams.count("autoplay")) {
            bool autoplay = (nextSceneParams["autoplay"] == "true");
            m_gameManager->setAutoPlay(autoplay);
        }
        
        if (currentScene) {
            currentScene->initialize();
            sceneStartTime = std::chrono::steady_clock::now();
        }
        
        transitioning = false;
    }
};
```

**Beispiel für die Verwendung:**

```cpp
// TitleScene.h
class TitleScene : public Scene {
public:
    void initialize() override {
        // 3D-Logo, Sternenhintergrund und Feuerball initialisieren
    }
    
    void update(float deltaTime) override {
        // Animation des Logos und des Feuerballs aktualisieren
    }
    
    void render(GLuint targetFBO) override {
        // Rendern des 3D-Logos, Sternenhintergrunds und Feuerballs
    }
    
    void cleanup() override {
        // Ressourcen freigeben
    }
    
    int getDuration() const override { 
        return 5000; // 5 Sekunden zeigen, dann automatisch weiter
    }
    
    std::string getNextSceneName() const override {
        return "Game"; // Nach der Titelszene zur Spielszene wechseln
    }
    
    std::map<std::string, std::string> getNextSceneParams() const override {
        return {{"autoplay", "true"}}; // Spielszene im Autoplay-Modus starten
    }
};

// GameScene.h
class GameScene : public Scene {
public:
    void initialize() override {
        // Spielumgebung initialisieren
    }
    
    int getDuration() const override { 
        return 10000; // 10 Sekunden im Autoplay-Modus
    }
    
    std::string getNextSceneName() const override {
        return "Highscore";
    }
};

// Main.cpp
void setupScenes() {
    SceneManager sceneManager(screenWidth, screenHeight, effectFactory, gameManager);
    
    // Szenen registrieren
    sceneManager.registerScene("Title", std::make_unique<TitleScene>());
    sceneManager.registerScene("Game", std::make_unique<GameScene>());
    sceneManager.registerScene("Highscore", std::make_unique<HighscoreScene>());
    sceneManager.registerScene("Credits", std::make_unique<CreditsScene>());
    
    // Mit der Titelszene beginnen
    sceneManager.changeScene("Title", TransitionEffectType::FadeIn);
    
    // Hauptschleife
    while (running) {
        // ...
        sceneManager.update(deltaTime);
        sceneManager.render();
        // ...
    }
}
```

Dieses Design unterstützt:

- Vollständig interaktive Szenen mit 3D-Inhalten
- Automatische Übergänge mit Effekten
- Jede Szene kann ihre Nachfolgeszene und Parameter definieren
- Integration mit dem GameManager für Autoplay-Funktionalität
- Flexible Konfiguration und Ablaufsteuerung
