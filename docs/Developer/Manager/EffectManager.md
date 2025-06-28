# EffectManager

Vereinfachtes Beispiel für den EffectManager

```c++
class EffectManager {
public:
EffectManager(EventManager* eventManager, TextureManager* textureManager)
: m_eventManager(eventManager), m_effectFactory(textureManager) {

        // Auf GameEvents hören
        m_eventManager->subscribe<BrickDestroyedEvent>([this](const BrickDestroyedEvent& event) {
            // Brick wurde zerstört, erzeuge Funken-Effekt
            auto effect = m_effectFactory.createSparkEffect(event.position);
            m_activeEffects.push_back(std::move(effect));
        });
        
        // Weitere Event-Handling...
    }
    
    // Update und Draw-Methoden...

private:
EventManager* m_eventManager;
EffectFactory m_effectFactory;
std::vector<std::unique_ptr<ParticleEffect>> m_activeEffects;
};
```

![EffectManager.svg](EffectManager.svg)