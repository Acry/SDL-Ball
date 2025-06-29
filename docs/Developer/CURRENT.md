# Current issues

Top: Fix all current manual Tests

- [ ] EffectsManagerTests (trigger event at mouse position)

- [ ] Paddle/PaddleManager real implementations in src
- [ ] Ball/BallManager real implementations in src

- [ ] PowerupManager

- [ ] CollisionManager

- [ ] Powerups
- [ ] Gun

Solve logic issue kill, collide, render:
Paddle-Spawn, Paddle Despawn - on level completed, Paddle explode

- [X] rethink collision (paddle collision points)
  Mixed approach for now, on collision in GameObjectManagers
  no convex paddle for now, just a simple rectangle, set angle

- [ ] [Tracer.md](Effects/Tracer.md) (refactor)
- [ ] Tracer-Tests
- [ ] TracerManager-Tests

- [ ] Title Scene
- [ ] Title Scene Test

- [ ] GameManager mit Autoplay
- [ ] SceneManager -> FBO -> TransitionEffect Title Scene <-> Autoplay Game Scene

## Connect the dots:

- [ ] GameManager.md
- [ ] CodeManager.md
- [ ] SceneManager.md -> TransitionEffects

run codeManager (setup game), run sceneManager, (simple menuManager?) run gameManager

scene 1-2 for ten seconds, autoplay, then scene 3-4 for ten seconds, autoplay, then scene 5,6 for ten seconds, autoplay

images instead of scenes 1-6

ScoreManager
HudManager

MenuManager, HighScoreManager, SaveGameManager

PlayGame: key, or movement brings up Want to play Game? hides after 5 seconds
