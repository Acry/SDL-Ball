// game state
void initNewGame() {
player.level = 0;
player.score = 0;
gVar.deadTime = 0;

```c++
void initNewGame() {
    player.level = 0;
    player.score = 0;
    gVar.deadTime = 0;

    gVar.newLevel = true;
    gVar.newLife = true;

    player.multiply = 1;

    switch (player.difficulty) {
        case EASY:
            player.coins = 600;
            player.lives = 5;
            break;
        case NORMAL:
            player.coins = 300;
            player.lives = 3;
            break;
        case HARD:
            player.coins = 0;
            player.lives = 3;
            break;
        default: ;
    }

    resetPlayerPowerups();
}
```

`difficultyStruct fixed_difficulty, runtime_difficulty;`
