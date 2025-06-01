// Brick-Typen als Enum für bessere Lesbarkeit
enum class BrickType : char {
None = '0', // Kein Brick
Blue = '1', // Blauer Brick
Yellow = '2', // Gelber Brick
Cement = '3', // Zement/harter Brick
Glass = '4', // Glas-Brick (braucht 2 Treffer)
Green = '5', // Grüner Brick
Grey = '6', // Grauer Brick
Purple = '7', // Lila Brick
White = '8', // Weißer Brick
Invisible = '9', // Unsichtbarer Brick
Red = 'A', // Roter Brick
Unused_B = 'B', // Unbenutzt
Doom = 'C', // Doom-Brick (erzeugt immer ein zufälliges böses Powerup)
Custom = 'D' // Farbiger Brick mit anpassbarer Farbe
};
