# Classic States

## States and Scenes

Die klassischen states, mh. Ich sehe die eher als hierrachische spielszenen statt:
`enum class State { RUNNING, PAUSED, GAME_OVER }; State currentState`;

SceneManager: switch - PlayScene, GameOverScene, TitleScene, (SettingsScene)
changeScene(std::move(pauseScene));
menuManager.showPauseMenu();

Pretty much, a top down approach.
