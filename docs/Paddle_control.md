# Verbesserung der Paddle-Steuerung

Die aktuelle Steuerung des Paddles ist tatsächlich problematisch:

1. Die direkte Positionsänderung bei Tastendruck ist nicht zeitabhängig
2. Die Mauskoordinaten werden falsch berechnet (mehrfach überschrieben)
3. Die aktuelle `update()`-Methode könnte verbessert werden

## Verbesserungsvorschlag

Hier ist ein verbesserter Ansatz für die Paddle-Steuerung:

```cpp
// In Paddle.h: Neue Methode hinzufügen
void moveTo(float targetX, float deltaTime);
```

```cpp
// In Paddle.cpp: Implementierung
void Paddle::moveTo(float targetX, float deltaTime) {
    // Bewegungsgeschwindigkeit basierend auf deltaTime
    float speed = 2.0f; // Anpassbare Geschwindigkeit
    float movement = speed * deltaTime;
    
    // Bewege in Richtung Zielposition
    if (pos_x < targetX) {
        pos_x = std::min(pos_x + movement, targetX);
    } else if (pos_x > targetX) {
        pos_x = std::max(pos_x - movement, targetX);
    }
    
    // Begrenzung auf den Bildschirmrand
    if (pos_x < -1.0f + width) pos_x = -1.0f + width;
    if (pos_x > 1.0f - width) pos_x = 1.0f - width;
}

// Update-Methode aktualisieren
void Paddle::update(float deltaTime) {
    updateGrowth(deltaTime);
}
```

## Verwendung im Test-Code

```cpp
// Bei Tastatureingabe
if (event.type == SDL_KEYDOWN) {
    float moveStep = 1.5f * deltaTime; // Geschwindigkeitsanpassung
    
    switch (event.key.keysym.sym) {
        case SDLK_LEFT:
            paddle.moveTo(paddle.pos_x - moveStep, deltaTime);
            break;
        case SDLK_RIGHT:
            paddle.moveTo(paddle.pos_x + moveStep, deltaTime);
            break;
    }
}

// Bei Mausbewegung
if (event.type == SDL_MOUSEMOTION) {
    // Berechne normalisierte Mauskoordinaten (-1.0 bis 1.0) - nur einmal
    normalizedMouseX = (event.motion.x - display.viewportX - display.viewportW / 2.0f) * 
                       (2.0f / display.viewportW);
    
    // Begrenze die Werte auf den gültigen Bereich
    normalizedMouseX = std::max(-1.0f + paddle.width, 
                       std::min(1.0f - paddle.width, normalizedMouseX));
    
    // Paddle zur Mausposition bewegen
    paddle.moveTo(normalizedMouseX, deltaTime);
}

// Im Hauptloop
paddle.update(deltaTime);
```

Diese Änderungen bieten folgende Vorteile:
- Zeitabhängige Bewegung für flüssige Animation
- Bessere Trennung der Verantwortlichkeiten
- Konsistente Begrenzungen für Maus- und Tastensteuerung
- Smoother Bewegungsablauf statt abrupter Positionswechsel