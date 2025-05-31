# SoundManager

## Weitere Möglichkeiten mit SDL_Mixer

SDL_Mixer bietet noch viele weitere Funktionen, die du in deinem Breakout-Spiel nutzen kannst:

### 1. Hintergrundmusik

```cpp
// Musik laden und mit Fade-In abspielen
Mix_Music *gameMusic = Mix_LoadMUS("../themes/default/music/game.mp3");
Mix_FadeInMusic(gameMusic, -1, 1000); // -1 = Endlosschleife, 1000ms Fade-In

// Sanfte Übergänge zwischen Spielzuständen
void switchToMenuMusic() {
    Mix_FadeOutMusic(800); // Ausblenden in 800ms
    Mix_FadeInMusic(menuMusic, -1, 1000); // Einblenden in 1s
}
```

### 2. Kanalgruppierung für bessere Verwaltung

```cpp
// Initialisierung: Kanäle in Gruppen einteilen
void SoundManager::initChannelGroups() {
    // Gruppe 1: Kollisionen (Kanäle 0-3)
    for (int i = 0; i < 4; i++) Mix_GroupChannel(i, 1);
    
    // Gruppe 2: Power-Ups (Kanäle 4-7)
    for (int i = 4; i < 8; i++) Mix_GroupChannel(i, 2);
    
    // Gruppe 3: UI-Sounds (Kanäle 8-10)
    for (int i = 8; i < 11; i++) Mix_GroupChannel(i, 3);
}

// Nutzung: Freien Kanal aus bestimmter Gruppe finden
int channel = Mix_GroupAvailable(1); // Freien Kanal aus Gruppe 1 suchen
if (channel == -1) {
    // Wenn kein Kanal frei ist, ältesten Kanal dieser Gruppe nehmen
    channel = Mix_GroupOldest(1);
}
```

### 3. Kanalkontrolle und Callbacks

```cpp
// Ende eines Sounds überwachen
Mix_ChannelFinished([](int channel) {
    SDL_Log("Sound auf Kanal %d beendet", channel);
    // Hier ggf. Verkettung von Sounds oder Effekten
});

// Sounds sanft auslaufen lassen statt abrupt zu stoppen
Mix_ExpireChannel(channel, 500); // Sound in 500ms ausfaden
```

### 4. Erweiterte räumliche Positionierung

```cpp
// Verbesserte Position mit Winkel und Distanz
void playSoundWithPosition(int soundId, float x, float y) {
    int channel = Mix_PlayChannel(-1, sample[soundId], 0);
    if (channel != -1) {
        // Winkel berechnen (0° vorne, 90° rechts, 180° hinten, 270° links)
        int angle = static_cast<int>((x + 1.0f) * 90.0f); // -1...+1 zu 0...180
        
        // Distanz (0=nah...255=fern)
        int distance = static_cast<int>((1.0f - ((y + 1.0f) / 2.0f)) * 200);
        
        Mix_SetPosition(channel, angle, distance);
    }
}
```

### 5. Sound-Effekte

```cpp
// Echo-Effekt für bestimmte Sounds (z.B. in großen Höhlen-Levels)
void addEchoEffect(int channel) {
    // Implementierung eines Echo-Effekts
    // Erfordert eigene Callback-Funktionen
    Mix_RegisterEffect(channel, echoEffect, echoEffectDone, nullptr);
}
```

### 6. Spezielles für Breakout

```cpp
// Intensität der Kollisionssounds basierend auf Ballgeschwindigkeit
void playCollisionSound(int soundId, float x, float y, float velocity) {
    int volume = static_cast<int>(MIX_MAX_VOLUME * std::min(velocity / 2.0f, 1.0f));
    Mix_VolumeChunk(sample[soundId], volume);
    queueSound(soundId, x, y);
}

// Power-Up-Sounds mit speziellen Effekten
void playPowerUpSound(int powerUpType) {
    int channel = Mix_PlayChannel(-1, sample[SND_GOOD_PO_HIT_PADDLE], 0);
    
    // Je nach Power-Up-Typ andere Effekte anwenden
    switch (powerUpType) {
        case POWERUP_ENLARGE:
            // Tieferer Pitch-Effekt für "größer werden"
            break;
        case POWERUP_SPEED:
            // Höherer Pitch-Effekt für "schneller werden"
            break;
    }
}
```

## Zusätzliche Funktionen II

Diese Funktionen könnten dein Audioerlebnis deutlich verbessern und den Spielern ein immersiveres Erlebnis bieten.
Welche dieser Funktionen interessieren dich besonders für die Implementierung?

// Kanäle für bestimmte Soundtypen gruppieren
Mix_GroupChannel(0, 1); // Kanal 0 der Gruppe 1 zuweisen (z.B. Kollisionen)
Mix_GroupChannel(1, 1);
Mix_GroupChannel(2, 2); // Kanal 2 der Gruppe 2 zuweisen (z.B. Power-Ups)

// Alle Sounds einer Gruppe stoppen (z.B. beim Level-Ende)
Mix_HaltGroup(1);

Erweiterte 3D-Positionierung

// Statt nur Panning auch Entfernung simulieren
// angle: 0=vorne, 90=rechts, 180=hinten, 270=links
// distance: 0=nah, 255=fern
Mix_SetPosition(channel, angle, distance);

// Nur Entfernung setzen (Lautstärkeabfall)
Mix_SetDistance(channel, distance);

// Callback bei Kanalende
Mix_ChannelFinished([](int channel) {
SDL_Log("Sound auf Kanal %d beendet", channel);
// Hier ggf. neue Sounds auslösen
});

// Benutzerdefinierte Audioeffekte (z.B. Echo)
Mix_RegisterEffect(channel, echoEffectFunction, echoEffectDone, echoUserData);

FADING

// Bei zu vielen gleichzeitigen Sounds, ältere auslaufen lassen
Mix_ExpireChannel(oldestChannel, 500); // In 500ms beenden

// Laufende Sounds pausieren (z.B. im Pausemenü)
Mix_Pause(-1); // Alle Kanäle pausieren
Mix_Resume(-1); // Alle Kanäle fortsetzen

## Integration von SoundManager mit EventManager

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
