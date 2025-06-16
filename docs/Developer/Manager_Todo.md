## Missing Managers

Ball,
Paddle,
SceneManager:
Autoplay, Credits Hauptmenü, Spiel, Pause, Game Over)
SaveGameManager,
HighScoreManager,
MenuManager
HudManager: clock, leben, score, level

## GameObject Managers

### collide

Die Init-Methode ist in den Actors m.E. nicht nötig. Von Beginn an war im Hinterkopf, dass die GameObjects/Actors
so wenig wie möglich Logik enthalten, weil die Manager-Klassen die Logik übernehmen sollen.

Wir könnten ICollideable auf die grundlegenden geometrischen Eigenschaften reduzieren und die Kollisionslogik in die
Manager verschieben.

```c++
// ICollideable.h
#pragma once

class ICollideable {
public:
    virtual ~ICollideable() = default;

    // Nur grundlegende Geometrie
    virtual float getPosX() const = 0;
    virtual float getPosY() const = 0;
    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;
    virtual bool isActive() const = 0;
};
```

## Texture Manager

TextureManager event, emit Sprites cached?

## SoundManager

use enum, no define
Sound test: write current sound

## Auxiliary Managers

- [ ] [ShaderManager](Manager/ShaderManager.md)