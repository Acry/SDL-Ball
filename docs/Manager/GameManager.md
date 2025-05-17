# GameManager

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