```c++
if (!var.paused)
    gVar.deadTime += globalTicks;
```

Der `gVar.deadTime` Counter wird verwendet um festzustellen wie lange der Spieler schon in einem Level feststeckt.

Nach 20 Sekunden (20000ms) wird den Bällen die "Explosive"-Fähigkeit gegeben um Zementblöcke zerstören zu können.

Dies ist eine Art "Anti-Frustrations"-Feature:

```c++
if (gVar.deadTime > 20000) {
    bMan.powerup(PO_EXPLOSIVE); 
    // give the balls explosive ability, in order to blow up cement block and get on with the game
}
```

Der Counter wird mit jedem Frame erhöht solange das Spiel nicht pausiert ist
Nach 20 Sekunden ohne Fortschritt werden die Bälle explosiv
Dies ermöglicht es dem Spieler, blockierende Zementblöcke zu zerstören
Der Counter wird bei Levelstart auf 0 zurückgesetzt: gVar.deadTime = 0;
Der Name "deadTime" bezieht sich also auf die Zeit, in der der Spieler in einer "Sackgasse" feststeckt.
