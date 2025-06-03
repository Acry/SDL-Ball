# Powerup-Concept

Aus der README entnehme ich diese wichtigen Informationen über das Powerup-Konzept:

1. Grundprinzip:

- Beim Treffen von Bricks gibt es Powerups
- Powerups geben dem Spieler "Coins" (die genaue Anzahl ist für den Spieler nicht sichtbar)
- Mit den Coins können wiederum neue Powerups gekauft werden

2. Steuerung für Powerups:
   Verschiedene Eingabemöglichkeiten:

- Tastatur:
  - v: Nächstes Powerup auswählen
  - n: Vorheriges Powerup auswählen
  - b: Ausgewähltes Powerup kaufen

- Maus:
  - Mouse Up/Down: Powerup auswählen
  - Rechtsklick: Powerup kaufen

- WiiMote:
  - Button 1: Item kaufen
  -
    + links/rechts: Powerup auswählen

- Joystick:
  - Button 2: Powerup kaufen
  - Y-Achse: Powerup auswählen

Das passt sehr gut zu der bereits implementierten Architektur mit:

- EventManager für die Eingabeverarbeitung
- PowerupManager für die Verwaltung
- BrickManager für das Spawnen von Powerups bei Brick-Zerstörung

Dies legt eine Event-basierte Integration nahe, wo der PowerupManager auf Brick-Zerstörungen und Eingabeevents reagiert.

## old code

MAXPOTEXTURES

Array von maximal MAXPOWERUPS PowerUp-Objekten
PowerUps haben eine Position, Geschwindigkeit und Gravitation
Es gibt einen aktiv/inaktiv Status für jedes PowerUp

clear(): Setzt alle PowerUps auf inaktiv
spawn(): Erzeugt neue PowerUps mit Position und Geschwindigkeit
move(): Bewegt aktive PowerUps
draw(): Zeichnet aktive PowerUps
coldet(): Prüft Kollisionen mit dem Paddle
die(): Deaktiviert PowerUps mit Effekten

'1' - PO_GROWPADDLE (Paddle vergrößern)
'2' - PO_SHRINKPADDLE (Paddle verkleinern)
'3' - PO_DIE (Tod)
'4' - PO_GLUE (Kleben)
'5' - PO_MULTIBALL (Mehrere Bälle)
'6' - PO_THRU (Durchschuss)
'7' - PO_DROP (Fallen)
'8' - PO_DETONATE (Detonieren)
'9' - PO_EXPLOSIVE_GROW (Explosive wachsen)
'A' - PO_EASYBRICK (Leichte Steine)
'B' - PO_EXPLOSIVE (Explosive)
'C' - PO_NEXTLEVEL (Nächstes Level)
'D' - PO_AIMHELP (Zielhilfe)
'E' - PO_COIN (Münze)
'F' - PO_BIGBALL (Großer Ball)
'G' - PO_NORMALBALL (Normaler Ball)
'H' - PO_SMALLBALL (Kleiner Ball)
'I' - PO_AIM (Zielen)
'O' - PO_LIFE (Leben)
'P' - PO_GUN (Waffe)
'R' - PO_LASER (Laser)

Punktewerte:
Verschiedene PowerUps geben unterschiedliche Punktzahlen
Positive (z.B. Multiball: 500) und negative (z.B. Shrink: -1000) Werte

Life (Leben):          1000
Multiball:             500
BigBall:               300
NormalBall:            400
SmallBall:             100
Aim (Zielen):          1600
GrowPaddle:            500
Explosive:             1400
Gun (Waffe):           1800
Thru (Durchschuss):    1000
Laser:                 500
Detonate:              1000
ExplosiveGrow:         1000
EasyBrick:             1000
NextLevel:             1000
AimHelp:               1000

ShrinkPaddle:         -1000
Die (Tod):            -1000
Drop (Fallen):        -1000

## Refactoring

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

dropChance(0.3f) // 30% Standardchance

```

//enum class PowerupType {
// Aim, // Zielhilfe
// AimHelp, // Zielhilfe-Hilfe
// BigBall, // Großer Ball
// Bullet, // Schuss
// Coin, // Münze
// Detonate, // Detonator
// Die, // Leben verlieren
// Drop, // Drop Powerup
// EasyBrick, // Einfacher Brick
// Explosive, // Explosiver Brick
// ExplosiveGrow, // Explosiver Wachstum
// Glue, // Kleber Powerup
// GoThrough, // Durchgehen Powerup
// GrowPaddle, // Paddle wachsen lassen
// Gun, // Waffe Powerup
// Laser, // Laser Powerup
// Life, // Extra Leben
// Multiball, // Mehrere Bälle
//};