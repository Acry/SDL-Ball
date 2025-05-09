# level

loadlevel
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
