# SDL Key States und kontinuierliche Tasteneingabe

Standardmäßig wird ein SDL_KEYDOWN-Event nur einmal ausgelöst, wenn eine Taste gedrückt wird, und nicht kontinuierlich
während die Taste gehalten wird.

Hier ist die Lösung:
SDL-Tastaturstatus direkt abfragen statt Events zu nutzen
Eine kleine Verzögerung einbauen um die Geschwindigkeit zu kontrollieren

```c++
// Am Anfang der main():
const Uint32 KEY_REPEAT_DELAY = 100; // Millisekunden zwischen Wiederholungen
Uint32 lastKeyPressTime = 0;

// In der Hauptschleife, nach dem Event-Handling:
const Uint8* keyState = SDL_GetKeyboardState(nullptr);
Uint32 currentTime = SDL_GetTicks();

// Prüfe ob genug Zeit seit dem letzten Tastendruck vergangen ist
if (currentTime - lastKeyPressTime >= KEY_REPEAT_DELAY) {
    if (keyState[SDL_SCANCODE_RIGHT]) {
        currentLevel++;
        if (currentLevel > levelCount) currentLevel = 1;
        levelManager.getBrickDataForLevel(currentLevel);
        lastKeyPressTime = currentTime;
    }
    else if (keyState[SDL_SCANCODE_LEFT]) {
        currentLevel--;
        if (currentLevel < 1) currentLevel = levelCount;
        levelManager.getBrickDataForLevel(currentLevel);
        lastKeyPressTime = currentTime;
    }
}
```

- Prüft den aktuellen Tastaturzustand direkt
- Führt die Aktion nur aus wenn genug Zeit vergangen ist
- Ermöglicht flüssiges Durchlaufen der Level beim Halten der Taste
- Verhindert zu schnelles Durchschalten durch die Verzögerung