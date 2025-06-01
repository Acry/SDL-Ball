# Level Format

Ein Level beginnt mit "**Start \**" und endet mit "** Stop **".
Es gibt 23 Zeilen mit grundsätzlich je 26 Spalten (52 Zeichen) pro Level.

## Brick-Format

Ein Brick wird durch zwei Zeichen beschrieben:
PB (PoweruptypBricktyp)
P = Powerup-Typ (0-R)
B = Brick-Typ (0-D)

"00" bedeutet leerer Platz

Ein normaler Brick ohne Powerup wird typischerweise als "0X" dargestellt, wobei X der Brick-Typ ist. Beispiele:

01 = blauer Brick ohne Powerup
02 = gelber Brick ohne Powerup
08 = weißer Brick ohne Powerup
0A = roter Brick ohne Powerup

Besondere Brick-Typen
03 = Zement/harter Brick
04 = Glas-Brick (braucht 2 Treffer)
09 = unsichtbarer Brick (wird erst nach dem ersten Treffer sichtbar, braucht dann 2 weitere Treffer)
0C = Doom-Brick (erzeugt immer ein zufälliges böses Powerup)
0D = Farbiger Brick mit anpassbarer Farbe (gefolgt von 6 Zeichen für RGB-Hexwerte)

### Spezialfall: Farbige Bricks (Typ D)

Wenn der Bricktyp 'D' ist, folgen 6 zusätzliche Zeichen für RGB-Farben im Hex-Format:
PD RRGGBB || |||||| || ||||++ Blau (00-FF) || ||++-- Grün (00-FF) || ++---- Rot (00-FF) |+------- Typ D (farbiger Brick) +-------- Powerup
Beispiel: `0DFFFFFF` = weißer Brick ohne Powerup

In den Leveldateien sehen Sie Muster wie M3, M5, etc. - dies sind Bricks mit zufälligen Powerups (M = 2% Chance) und
verschiedenen Brick-Typen.

## Level-Steuerung

Spezielle Kommandos können nach "** Start **" eingefügt werden:

## Old Description

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
