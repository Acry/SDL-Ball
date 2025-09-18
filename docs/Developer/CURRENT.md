# Current issues

Top: test-driven GameManager

rethink: BallManager::checkBorderCollision()

- [ ] [GameManager.md](Manager/GameManager.md) real implementations in src
- [ ] [EffectManager](Manager/EffectManager.md) real implementations in src

## Lateral

- [ ] Fix clang warnings
- [ ] fix `cppcheck --enable=unusedFunction ./src` warnings

- [X] rethink collision (paddle collision points)
  Mixed approach for now, on collision in GameObjectManagers
  no convex paddle for now, just a simple rectangle, set angle

Brick-> shrink away on life lost or explode

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
