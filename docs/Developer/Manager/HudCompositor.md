# HudManager

## Design

Der HudManager zeigt Merkmale von folgenden Pattern:

- Adapter: Er vereinheitlicht die Schnittstellen verschiedener Manager und macht sie für die HUD-Logik nutzbar.
- Brücke: Er trennt die Abstraktion (HUD-Logik) von der Implementierung (TextManager, TextureManager, EventManager).
  Da er die HUD-Elemente zusammenführt und deren Implementierungen abstrahiert, ist er eine Mischform und kann als
  Kombination aus Adapter und Brücke betrachtet werden.
  Wenn der HudManager verschiedene HUD-Elemente wie Uhr, Leben, Score usw. verwaltet und diese als einzelne Objekte oder
  als Baumstruktur behandelt, erfüllt er auch das Composite-Pattern. Dadurch kann der Manager sowohl einzelne Elemente
  als auch Gruppen von Elementen einheitlich ansprechen und verarbeiten.

## Components

clock, lives, score, current level, speedometer, shop

## Todo

- [ ] Level
- [ ] Powerups consumed (bottom)
- [ ] Shop (top)
- [ ] Speedometer
- [ ] Events (Keys)
