# Powerup

```c++
// Powerup-Typen als Enum für bessere Lesbarkeit
enum class PowerupType : char {
    None = '0',           // Kein Powerup
    Extrude = '1',        // Paddle verbreitern
    Shrink = '2',         // Paddle verkleinern
    Die = '3',            // Sofort sterben
    Glue = '4',           // Ball klebt am Paddle
    MultiBalls = '5',     // Mehrere Bälle
    GoThru = '6',         // Ball durchdringt Bricks
    BoardDown = '7',      // Brett fällt eine Stufe runter
    DetonateExplosives = '8', // Alle Explosiv-Bricks zünden
    GrowExplosives = '9', // Explosive Bricks werden größer
    OneHitDestroy = 'A',  // Alle Bricks in einem Treffer zerstörbar
    Explosive = 'B',      // Explosiver Powerup
    NextLevel = 'C',      // Nächstes Level
    AimHelper = 'D',      // Zeigt Abprallwinkel
    Coins1000 = 'E',      // 1000 Münzen
    BigBall = 'F',        // Großer Ball
    NormalBall = 'G',     // Normaler Ball
    SmallBall = 'H',      // Kleiner Ball
    Aim = 'I',            // Zielhilfe
    ExtraLife = 'O',      // Extra Leben
    Gun = 'P',            // Schusswaffe
    LaserSight = 'R',     // Laser-Zielhilfe
    
    // Zufällige Powerups mit verschiedenen Wahrscheinlichkeiten
    RandomPowerup10 = 'J', // 10% Chance
    RandomPowerup100 = 'K', // 100% Chance
    RandomPowerup5 = 'L',  // 5% Chance
    RandomPowerup2 = 'M',  // 2% Chance
    RandomPowerup1 = 'N',  // 1% Chance
    RandomEvilPowerup = 'Q' // 100% Böses Powerup
};
```
