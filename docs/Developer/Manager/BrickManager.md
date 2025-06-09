# BrickManager

Event-Flow:

CollisionManager erkennt die Kollision zwischen Ball und Brick

Erstellt CollisionData mit:
data.object1 = &obj1; // Ball
data.object2 = &obj2; // Brick
data.hitX = obj1.getPosX();
data.hitY = obj1.getPosY();
Emittiert GameEvent::BallHitBrick mit diesen Daten

BrickManager empfängt das Event durch seinen registrierten Listener:

eventManager->addListener(GameEvent::BallHitBrick,
[this](const CollisionData& data) { onBallHitBrick(data); },
this);

BrickManager kann dann:
Den getroffenen Brick identifizieren (data.object2)
Die Kollisionsstelle auswerten (data.hitX/Y)
Entsprechende Aktionen ausführen (z.B. Brick zerstören, Brick anzeigen, Score senden)

```c++
void BrickManager::onBallHitBrick(const CollisionData& data) {
    const auto *brick = static_cast<const Brick*>(data.object2);
    const auto index = static_cast<size_t>(brick - &bricks[0]);

    EventData hitData;
    hitData.target = &bricks[index];
    hitData.posX = data.hitX;
    hitData.posY = data.hitY;
    onBrickHit(hitData);
}
```
