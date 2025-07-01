# Current issues

Top: Fix all manual tests warnings

Tests:

- [ ] EffectManager real implementations in src
- [ ] Paddle/PaddleManager real implementations in src
- [ ] Ball/BallManager real implementations in src

- [ ] GameManager

- [ ] PowerupManager
- [ ] Powerups

- [ ] Gun

- [X] rethink collision (paddle collision points)
  Mixed approach for now, on collision in GameObjectManagers
  no convex paddle for now, just a simple rectangle, set angle

- [ ] GameManager mit [Autoplay](misc/Autoplay.md)

- [ ] [Tracer.md](Effects/Tracer.md) (refactor)
- [ ] Tracer-Tests
- [ ] TracerManager-Tests

Brick-> shrink away on life lost or explode

Solve logic issue kill, collide, render:
Paddle-Spawn, Paddle Despawn - on level completed, Paddle explode

## Connect the dots:

- [ ] [CodeManager.md](Manager/CodeManager.md) ->
- [ ] [SceneManager.md](Manager/SceneManager.md) -> TransitionEffects
- [ ] [GameManager.md](Manager/GameManager.md) -> Game
-

run codeManager (setup game), run sceneManager, (simple menuManager?) run gameManager

ScoreManager
HudManager

MenuManager, HighScoreManager, SaveGameManager

PlayGame: key, or movement brings up Want to play Game? hides after 5 seconds

- [ ] HudManager
- [ ] MenuManager
- [ ] Title Scene
- [ ] Title Scene Test
- [ ] SplashScene -> music
- [ ] SceneManager -> FBO -> TransitionEffect Title Scene <-> Autoplay Game Scene