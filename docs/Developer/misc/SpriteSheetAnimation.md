# Die Rolle von SpriteSheetAnimation im Projekt

```cpp
void resetAllAnimations(std::vector<SpriteSheetAnimation*> &animations) {
    for (auto *animation : animations) {
        if (animation) {
            animation->reset();
        }
    }
}
```

Die Klasse `SpriteSheetAnimation` und die zugehörige Implementierung in `SpriteSheetAnimation.cpp` spielen eine zentrale
Rolle im Texturmanagement-System des Projekts.
Die Klasse verwaltet insbesondere animierte Texturen mit folgenden Hauptfunktionen:

- Die Texture-Klasse ist primär für die Animation von Sprite-Sheets zuständig
- Die Hauptmethode play() kümmert sich um Frame-Wechsel und Texturkoordinaten-Berechnung
- Die Klasse verwaltet Animationszustände (Richtung, aktueller Frame, Timing)

1. **Verwaltung von Animationszuständen:**
    - Verfolgt den aktuellen Frame einer Animation
    - Handhabt die Aktualisierung der Frames basierend auf Zeitintervallen (Ticks)
    - Unterstützt bidirektionale Animationen (vorwärts und rückwärts)

2. **Berechnung von Texturkoordinaten:**
    - Berechnet die korrekten UV-Koordinaten für jeden Frame einer Spritesheet-Textur
    - Unterstützt Padding zwischen einzelnen Frames
    - Organisiert Frames in einem Raster (Zeilen und Spalten)

3. **Animation und Wiedergabe:**
    - Die `play()`-Methode ist das Herzstück der Klasse
    - Aktualisiert den aktuellen Frame basierend auf verstrichener Zeit
    - Berechnet die Texturkoordinaten für den aktuellen Frame

Die Klasse arbeitet eng mit `TextureManager` zusammen, der für das Laden und die grundlegende Verwaltung von Texturen
zuständig ist, während `Texture` selbst für die Animation und die Berechnung der korrekten Texturkoordinaten beim
Rendern verantwortlich ist.

Die `textureProperties`-Struktur enthält alle notwendigen Informationen über die Textur, wie Größe, Layout des
Spritesheets, Animationsgeschwindigkeit und mehr.

___

Das SpriteSheetAnimation-System ist eine Komponente, die für die Animation von Sprites in einem 2D-Spiel zuständig ist.
Es verwendet Spritesheet-Texturen, die mehrere Animationsframes in einem einzigen Bild enthalten.

Hauptfunktionalitäten
Frame-basierte Animation: Das System verwaltet den aktuellen Frame der Animation und wechselt zwischen Frames basierend
auf einem Zeitintervall.
Bidirektionale Animation: Es unterstützt sowohl normale als auch bidirektionale Animationen (hin und zurück).
Texturkoordinaten: Berechnet automatisch die Texturkoordinaten für jeden Frame aus einem Spritesheet.
Konfiguration durch Dateien: Animationsparameter werden über Konfigurationsdateien wie glue.txt definiert.
Konfigurationsparameter (wie in glue.txt)
ticks: Zeitintervall zwischen Frames in Millisekunden
cols/rows: Anordnung der Frames im Spritesheet
xoffset/yoffset: Offset zwischen Frames
frames: Gesamtzahl der Frames
playing: Animation automatisch starten (0=nein, 1=ja)
bidir: Bidirektionale Animation (0=nein, 1=ja)
padding: Padding zwischen Frames aktivieren (0=nein, 1=ja)
Code-Struktur
Die Animation wird durch die play()-Methode vorangetrieben, die den Zeitfortschritt (deltaTime) nutzt, um zu bestimmen,
wann ein Framewechsel stattfinden soll. Wenn ein neuer Frame aktiv wird, werden die Texturkoordinaten für die vier
Eckpunkte des Sprites neu berechnet.

Die Klasse ist Teil eines größeren Systems, das laut Makefile vermutlich für ein Spiel mit Elementen wie Paddle, Ball
und Spielfeldbegrenzungen verwendet wird.

Normale Animation (mit bidir=0) - spielt von Frame 1 bis frames und springt dann zurück zu Frame 1

Bidirektionale Animation (mit bidir=1) - spielt von Frame 1 bis frames und dann rückwärts zurück zu Frame 1

Das System bietet keine eingebaute Funktionalität für:

## Einmaliges Abspielen und dann Stop

Callback bei Animation-Ende
Automatisches Anhalten nach einem Durchlauf
Um ein einmaliges Abspielen zu implementieren, müsstest du den Code etwa so erweitern:

class SpriteSheetAnimation {
float age;
bool direction;
Uint32 lastFrame;
bool playOnce; // Neuer Flag für einmaliges Abspielen
bool completed; // Status, ob einmaliges Abspielen abgeschlossen ist

public:
void play(float deltaTime = 0.0f);
void setPlayOnce(bool playOnce); // Neue Methode
bool isCompleted() const; // Neue Methode
// ... restliche existierende Member
};

Die Implementierung der play()-Methode müsste dann angepasst werden, um nach einem Durchlauf die Animation zu stoppen.

## Akkumulation der frametime durch caching

Das Problem entsteht, weil für mehrere Objekte denselben SpriteSheetAnimation-Pointer aus dem Cache verwendest. Dadurch
teilen sich alle Instanzen die Animationsdaten (frame, age, etc.), sodass ein play()-Aufruf für eine Instanz auch die
Animation der anderen beeinflusst.
Das führt dazu, dass die Animation scheinbar schneller läuft, je mehr Objekte diese Texture teilen.

Lösung:
Jedes animierte Objekt sollte eine eigene Instanz von SpriteSheetAnimation besitzen, aber auf die gleiche
OpenGL-Textur (GLuint texture) zeigen. Nur die Textur (das Bild) sollte geteilt werden, nicht die Animationslogik.

Vorgehen:
Im Cache speicherst du nur die geladene Textur und die statischen Eigenschaften (z.B. Breite, Höhe, Farbwerte).
Für jedes Objekt erstellst du eine eigene SpriteSheetAnimation-Instanz und kopierst die statischen Eigenschaften aus dem
Cache, aber nicht die Animationszustände (frame, age, etc.).

Beispiel für eine Factory-Methode:

```cpp
// SpriteSheetAnimationFactory.h
#pragma once
#include "TextureManager.h"

class SpriteSheetAnimationFactory {
public:
   static SpriteSheetAnimation create(const SpriteSheetAnimation& cached) {
   SpriteSheetAnimation anim;
   anim.textureProperties = cached.textureProperties; // kopiert nur die statischen Eigenschaften
   // Animationszustände bleiben individuell
   return anim;
}
};
```

```cpp
auto* cached = textureManager.getPaddleTexture(PaddleTexture::Base);
SpriteSheetAnimation myAnim = SpriteSheetAnimationFactory::create(*cached);
```

Das Ziel ist: Jeder Brick soll eine eigene Instanz von SpriteSheetAnimation besitzen, aber alle teilen sich die gleiche
OpenGL-Textur (aus dem Cache des TextureManager).

```c++

// Brick.h (nur relevante Teile)
#include "SpriteSheetAnimation.h"

class Brick final : public GameObject, public ICollideable {
public:
// ...
SpriteSheetAnimation animation; // <- Jede Instanz hat ihre eigene Animation

    void update(float deltaTime) override {
        animation.play(deltaTime); // Animation pro Brick updaten
    }

    void draw(float deltaTime) override {
        // ... OpenGL-Rendering mit animation.textureProperties.texture und animation.texturePosition ...
    }
    // ...
};
```

BrickManager: Animation pro Brick initialisieren

```c++
// BrickManager.cpp (nur relevante Teile)
#include "BrickManager.h"

void BrickManager::setupBricks(const std::vector<BrickInfo> &data) {
bricks.clear();
for (const auto &info : data) {
Brick brick;
// ... Position, Größe, Typ setzen ...

        // Hole die gecachte Animation (enthält die Textur und statische Eigenschaften)
        BrickTexture texType = getTextureForType(info.type);
        SpriteSheetAnimation *cachedAnim = textureManager->getBrickTexture(texType);

        // Kopiere NUR die statischen Eigenschaften in die Brick-Animation
        if (cachedAnim) {
            brick.animation.textureProperties = cachedAnim->textureProperties;
        }

        bricks.push_back(std::move(brick));
    }
}
```

___

myAnim.texture = &cachedAnim->textureProperties.texture;

___

Texturressource (OpenGL-Ressource)
struct TextureResource {
GLuint id; // OpenGL Textur-ID
float pxw, pxh; // Pixel-Dimensionen für GL-Operationen
GLfloat textureColor[4]; // RGBA-Farbmodifikator
GLfloat particleColor[3]; // RGB-Partikelfarbe
std::string fileName; // Pfad zur Textur
};

struct SpriteSheetAnimationProperties {
float xoffset; // Größe eines Frames im UV-Raum
float yoffset; // Größe eines Frames im UV-Raum
int cols; // Anzahl der Spalten im Spritesheet
int rows; // Anzahl der Zeilen im Spritesheet
int ticks; // Zeit pro Frame (ms)
Uint32 frames; // Anzahl der Frames
bool bidir; // Bidirektionale Animation
bool playing; // Initial abspielen?
bool padding; // Anti-Bleeding Padding
};
