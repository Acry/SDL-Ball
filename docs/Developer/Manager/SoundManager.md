# SoundManager

# Integration von SoundManager mit EventManager

Dein SoundManager könnte stark vom EventManager profitieren, um Sounds automatisch bei bestimmten Spielereignissen
abzuspielen. Hier ist ein Vorschlag zur Integration:

1. **Referenz zum EventManager hinzufügen**:

```cpp
class SoundManager {
private:
    EventManager* eventManager = nullptr;
    // Bestehende Member...

public:
    // Bestehende Methoden...
    
    void registerEvents(EventManager* evManager);
};
```

2. **Event-Registrierung implementieren**:

```cpp
void SoundManager::registerEvents(EventManager* evManager) {
    if (!evManager) return;
    
    eventManager = evManager;
    
    // Kollisions-Events mit Sounds verknüpfen
    eventManager->addListener(GameEvent::BallHitLeftBorder, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX);
    }, this);
    
    eventManager->addListener(GameEvent::BallHitRightBorder, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX);
    }, this);
    
    eventManager->addListener(GameEvent::BallHitTopBorder, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX);
    }, this);
    
    eventManager->addListener(GameEvent::BallHitPaddle, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_PADDLE, data.posX);
    }, this);
    
    eventManager->addListener(GameEvent::BrickDestroyed, [this](const EventData& data) {
        this->queueSound(SND_NORM_BRICK_BREAK, data.posX);
    }, this);
    
    // Weitere Event-Listener hinzufügen...
}
```

3. **CollisionManager anpassen**:

Wie vorgeschlagen, sollte der CollisionManager Events auslösen:

```cpp
void CollisionManager::handleBallPaddleCollision(Ball& ball, const Paddle& paddle) {
    if (checkCollision(ball, paddle)) {
        // Physik berechnen wie bisher...
        ball.pos_y = paddle.getPosY() + paddle.height;
        // Weitere Berechnung...
        
        // Event auslösen
        EventData data;
        data.posX = ball.pos_x;
        data.posY = ball.pos_y;
        data.sender = &ball;
        data.target = &paddle;
        eventManager->emit(GameEvent::BallHitPaddle, data);
    }
}
```

4. **Bereinigung im Destruktor**:

```cpp
SoundManager::~SoundManager() {
    if (eventManager) {
        // Alle Event-Listener entfernen
        eventManager->removeListener(GameEvent::BallHitLeftBorder, this);
        eventManager->removeListener(GameEvent::BallHitRightBorder, this);
        eventManager->removeListener(GameEvent::BallHitTopBorder, this);
        eventManager->removeListener(GameEvent::BallHitPaddle, this);
        eventManager->removeListener(GameEvent::BrickDestroyed, this);
        // Weitere Events...
    }
    
    clearSoundTheme();
    Mix_CloseAudio();
    Mix_Quit();
}
```

5. **Verwendung im Hauptprogramm**:

```cpp
int main() {
    // Initialisierung...
    EventManager eventManager;
    SoundManager soundManager;
    
    // Sound-Theme laden
    soundManager.setSoundTheme("../themes/default/sound");
    
    // Events registrieren
    soundManager.registerEvents(&eventManager);
    
    // CollisionManager mit EventManager konfigurieren
    CollisionManager collisionManager(&eventManager);
    
    // Spielschleife...
    while (running) {
        // Logik und Kollisionsprüfung...
        
        // Sounds abspielen (am Ende des Frame)
        soundManager.play();
        
        // Rendering...
    }
    
    return 0;
}
```

Diese Lösung bietet eine saubere Trennung zwischen Kollisionserkennung, Event-Handling und Sound-Ausgabe. Der
SoundManager reagiert automatisch auf relevante Spielereignisse, ohne dass die Hauptschleife spezifischen Code dafür
enthalten muss.

___

Wir geben das Thema wieder von außen rein, hier das SoundThema.
Da wir bereits 2 Themen haben, lässt sich der Themenwechsel leicht testen.

Der SoundManager soll, wie andere Komponenten weder die Settings noch andere Komponenten kennen.
// if init return false, settingsManager should set setting.sound to false

Aus loadSounds wird, setSoundTheme, wie beim TextManager, das heißt auch

Taking out the stereo setting for now, one can add it back later; I don't see a point anymore.
