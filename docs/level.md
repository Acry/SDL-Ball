# level

## description from file:

```text
A level is started with the ** Start ** text, and ends with the ** Stop ** text.
There are 23 rows of bricks in a level.
There are 26 columns of bricks, that is 52 characters to describe one row of bricks.
One brick is described using two characters: PB (PoweruptypeBricktype)
00 is an empty space

Here is an example of a row consisting of a red brick with extrude powerup and a white brick with gun powerup, there are no other bricks in this row:

Powerup type Position
|Brick type position
||
|| Powerup Type position
|| |Brick type position
|| ||
1A P1 000000000000000000000000000000000000000000000000
|| ||
|| |1 = White brick
|| P = Gun Powerup
||
|A  = red brick
1  = Extrude powerup




Below is a description of powerup types and brick types.

0 = no powerup - ok
0 = no brick - ok
1 = extrude - ok
1 = blue colored brick - ok
2 = shrink - ok
2 = yellow colored brick - ok
3 = die - ok
3 = cement/hard brick - ok
4 = glue - ok
4 = glass brick (requires 2 hits to destroy) - OK
5 = multiply balls - ok
5 = green brick - ok
6 = go-thru all bricks powerup (ball is unstoppable and won't bounce off brick, but go thru them) - ok
6 = Grey - ok
7 = ball->paddle = board drops one down powerup - ok
7 = purple - ok
8 = detonate all explosives
8 = white brick.
9 = explosive bricks grow - ok
9 = invisible brick (turns visible first hit, then requires 2 additional hits) - OK
A = all bricks destructable in 1 hit powerup.-ok
A = red brick.
B = explosive (powerup) - ok
C = Next level powerup - ok
C = doom brik (This brick always spawns a random evil powerup)
D = Aim helper (show bounce-off angle)
D = custom color brick ( followed by RRGGBB in hex from 00 to FF ex. DFFFFFF = white, like html colors)
E = 1000 Coins Powerup.
F = BIGBALL powerup - ok
G = NORMALBALL powerup -ok
H = Smallball powerup - ok
I = aim powerup - ok
O = extra life - ok
P = GUN powerup - OK
R = Laser Sight (when player have glue and laser, he can see what direction ball will bounce off pad)- OK

J = random powerup  10% chance of getting a powerup when hitting this brick
K = random powerup 100%
L = random powerup   5%
M = random powerup   2%
N = random powerup   1%
Q = random evil powerup 100%

To create dropping levels, use this command, place it on it's own line on a new line after ** Start **
> down 10000
The above makes the bricks drop down once every 10 seconds.

Level 1
```

## loadlevel
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
