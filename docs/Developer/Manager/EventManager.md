# EventManager

Der EventManager verwaltet die Callbacks durch seine generischen Event-Handling-Methoden.

Der EventManager speichert Callbacks in seinen Maps:

```c++
std::unordered_map<GameEvent, std::vector<CollisionEventListenerEntry> > collisionEventListeners;
```

Du registrierst einfach deine Callback-Methode über addListener:

```c++
eventManager->addListener(GameEvent::BallHitBrick,
[this](const CollisionData& data) { onBallHitBrick(data); },
this);
```

Der EventManager ruft dann automatisch die registrierten Callbacks auf, wenn ein Event emittiert wird:

```c++
void EventManager::emit(const GameEvent event, const CollisionData &data) {
   auto it = collisionEventListeners.find(event);
   if (it != collisionEventListeners.end()) {
      for (const auto &entry: it->second) {
        entry.callback(data);
      }
   }
}
```

Dein EventManager folgt dem Observer-Pattern für ereignisbasierte Kommunikation.

- **Gute Trennung von Zuständigkeiten**: Events werden zentral verwaltet und verteilt
- **Typsichere Events** durch das `GameEvent`-Enum
- **Flexible Datenübertragung** mit `EventData`
- **Besitzer-Tracking** verhindert Memory-Leaks (Owner-Pointer)
- **Sicheres Event-Auslösen** durch Kopieren der Listener-Liste
- **Umfassendes Logging** für Debugging

### Verbesserungsmöglichkeiten:

1. **Integration von Tastaturereignissen**:
   ```cpp
   enum class GameEvent {
       // Bestehende Events...
       
       // Tastatur-Events
       KeyPressed,
       KeyReleased,
       KeyHeld
   };
   
   // EventData erweitern
   struct EventData {
       // Bestehende Felder...
       SDL_Keycode keyCode{0};
       Uint16 keyMod{0};
   };
   ```

2. **Event-Prioritäten**:
   ```cpp
   struct CallbackInfo {
       EventCallback callback;
       void* owner = nullptr;
       int priority = 0;  // Höhere Werte = frühere Ausführung
       
       // Konstruktor und operator() anpassen
   };
   ```

3. **Ereignis-Verzögerung**:
   ```cpp
   void emitDelayed(GameEvent event, const EventData& data, float delayInSeconds) {
       // Event in Warteschlange speichern und später auslösen
   }
   ```

4. **Verbesserung des Zusammenspiels mit CollisionManager**:
   Dein CollisionManager könnte Events für Kollisionen auslösen statt direkt Objekte zu aktualisieren:

   ```cpp
   void CollisionManager::handleBallPaddleCollision(Ball& ball, const Paddle& paddle) {
       if (checkCollision(ball, paddle)) {
           // Physik berechnen...
           
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

Der EventManager ist ein zentraler Baustein für saubere Spielarchitektur und ermöglicht lose Kopplung zwischen deinen
Komponenten. Die Integration von Tastaturereignissen würde die Konsistenz deines Event-Systems verbessern und
Eingabebehandlung flexibler gestalten.

```c++
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
```

## Notes

In C++ gibt es bereits etablierte Muster und Bibliotheken für Event-Dispatching:
Das Observer-Pattern ist das klassische Muster für Event-Dispatcher und Listener.

Die Standardbibliothek bietet mit std::function und std::map/std::unordered_map flexible Möglichkeiten, eigene
Dispatcher zu bauen.

Für komplexere Anforderungen gibt es Bibliotheken wie Boost.Signals2 (thread-sicher, flexibel) und libsigc++.

Ein typisierter, auf Spiel-Events spezialisierter Dispatcher wie dein EventManager ist aber oft projektspezifisch und
wird selten als Standardbibliothek angeboten. Die genannten Lösungen sind allgemeiner gehalten und können als Basis
dienen, aber deine Lösung ist für deine Anforderungen maßgeschneidert.

Du könntest das Observer-Pattern effizienter umsetzen, indem du die Listener-Typen vereinheitlichst (z.B. mit std::
function<void(const BaseEventData&)>) und die Event-Daten von einer gemeinsamen Basisklasse ableiten lässt. So sparst du
viele Maps und Methoden und erhöhst die Flexibilität.

Das Observer-Pattern wäre mit einer einzigen Listener-Liste und polymorphen Event-Daten noch klarer und wartbarer.

(z.B. BaseEventData) und die Listener als std::function<void(const BaseEventData&)> speicherst. Dadurch brauchst du nur
eine Listener-Liste und eine emit-Methode mit dynamischem Cast für die jeweiligen Event-Typen.

```c++
// EventManager.h
class BaseEventData {
   public:
   virtual ~BaseEventData() = default;
   };
   
using EventCallback = std::function<void(const BaseEventData&)>;

class EventManager final {
   std::unordered_map<GameEvent, std::vector<std::pair<void*, EventCallback>>> listeners;
   public:
   void addListener(GameEvent event, EventCallback callback, void* owner);
   void emit(GameEvent event, const BaseEventData& data);
   void removeListener(GameEvent event, void* owner);
};
```

Das reduziert Redundanz und macht die Erweiterung um neue Event-Typen einfacher.
Listener können die Event-Daten bei Bedarf per dynamic_cast auf ihren Typ prüfen.

## See also

[Oberserver Pattern.md](../Software-Engineering/Oberserver%20Pattern.md)
