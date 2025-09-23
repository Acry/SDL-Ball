# Current issues

Top: test-driven GameManager

- [ ] [GameManager.md](Manager/GameManager.md) real implementations in src
- [ ] [EffectManager](Manager/EffectManager.md) real implementations in src

- rename HudManager to HudCompositor
- rename EventDispatcher to InputEventDispatcher
- [HudTests](../../tests/manual/HudManager_Tests.cpp)) / [HudCompositor](Manager/HudCompositor.md): use events to emit
  score and lives
  Rethink event-data for score and live
  menu → clock, current level, speedometer, shop

rethink: BallManager::checkBorderCollision()

Refine depiction of the event-flow:
[MainLoop]
→ Events → InputEvents → InputEventDispatcher: emits inputEvents
Interface Input Device Manager receives inputEvents and emits GameEvents

Die Ballgeschwindigkeit wird durch den Schwierigkeitsgrad gesetzt.
Der Schwierigkeitsgrad kann durch die Settings geladen, oder das Menü verändert werden.
BallSpeed/MaxBallSpeed

Verwaltet werden diese Eigenschaften:
[BallSpeed, MaxBall Speed] → BallManager (current lowest Speed, current max Speed, current average Speed, overall
average Speed, max Speed, min Speed) -> Speedometer

[Leben/Level] → PaddleManager/SpielerManager

## Classes

The HudManager is no Manager class, because it lacks logic, it combines the hud elements and just reacts on events.

## Lateral

- [ ] Fix clang warnings
- [ ] fix `cppcheck --enable=unusedFunction ./src` warnings

- [X] rethink collision (paddle collision points)
  Mixed approach for now, on collision in GameObjectManagers
  no convex paddle for now, just a simple rectangle, set angle

Brick → shrink away on life lost or explode

Solve logic issue kill, collide, render:
Paddle-Spawn, Paddle Despawn - on level completed, Paddle explode
(physically active)

PlayGame: key, or movement brings up Want to play Game? hides after 5 seconds

- [ ] SceneManager
- [ ] Title Scene
- [ ] Title Scene Test

CodeManager->SceneManager

Connect: ThemeManager, SettingsManager

[ ] HighScoreManager
[ ] SaveGameManager
