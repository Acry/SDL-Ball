# GameManager

Der SettingsManager sollte hauptsächlich für das Laden, Speichern und Bereitstellen von Einstellungen verantwortlich sein, nicht für die Spielinitialisierung. Die zentrale GameManager-Klasse sollte das Management aller Subsysteme übernehmen.

settingsManager.initialize
themeManager.initialize
soundManager.initialize
textureManager.initialize
displayManager.initialize
gameObjectManager.initialize
gameObjectManager.loadLevel?

```c++
class GameSystem {
    public:
    void update(float deltaTime) {
        if (!paused) {
        // Alle Systeme aktualisieren
        for (auto& animation :animationComponents) {
            animation.update(deltaTime);
        }
        // Andere Systeme...
        }
    }
    void setPaused(bool isPaused) {
        this->paused = isPaused;
    }
};
```