// In der Initialisierungsfunktion des Spiels
void initGame() {
// Event-Listener für Sound-Events registrieren
eventSystem().addListener(GameEvent::BallHitBorder, [](const EventData& data) {
soundManager.add(data.soundID, data.posX);
});

    eventSystem().addListener(GameEvent::BallLost, [](const EventData& data) {
        // Aktion bei verlorenem Ball (z.B. Sound abspielen, Leben reduzieren)
        // soundManager.add(SND_BALL_LOST, data.posX);
    });
}