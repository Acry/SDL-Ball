# level logic

level_ranges[0] = Level 1
level_ranges[1] = Level 2

## dropping levels

scrollInfoScruct scrollInfo.drop

## levels complete

struct vars {
int numlevels;

if (player.level == var.numlevels) {
int numlevels;

## load level

themes/levels and powerup

single call of loadlevel

```c++
            if (gVar.newLevel) {
                var.bricksHit = true;
                gVar.newLevel = false;
                loadlevel(levelfile, bricks, player.level);
                initlevels(bricks, texLvl);
                gVar.gameOver = false;
                gVar.newLife = true;
                pMan.clear();
                bullet.clear();
                paddle.posx = 0.0;
                var.startedPlaying = false;
                bg.init(texMgr);
                hud.clearShop();
            }
```

wie wird newLevel gesetzt?
initNewGame

if (gVar.nextlevel)
gVar.newLevel = true;

main
string levelfile = themeManager.getThemeFilePath("levels.txt", setting.lvlTheme);

## bricks

```c++
#include <vector>

// Statt: brick bricks[598];
std::vector<brick> bricks;

// Beim Laden eines Levels:
void loadLevel(const LevelData& levelData) {
    bricks.clear();
    for (const auto& brickInfo : levelData.bricks) {
        brick b;
        // b entsprechend brickInfo initialisieren
        bricks.push_back(b);
    }
}
```
