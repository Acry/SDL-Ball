# Effects

![EffectManager.svg](../Manager/EffectManager.svg)

Schlüsselkomponenten und Beziehungen
Basisklassen:

BaseEffect: Abstrakte Basisklasse mit grundlegenden Methoden (init, update, draw, isActive)
BaseEffectProperties: Grundlegende Eigenschaften aller Effekte (Kategorie, Lebensdauer, Größe, Farbe)
Partikelspezifische Komponenten:

Particle: Einzelne Partikel, die in Effekten verwendet werden
ParticleEffectProperties: Erweitert die Basiseigenschaften um partikelspezifische Attribute (Typ, Anzahl,
Geschwindigkeit, Ausbreitung)
ParticleEffect: Implementierung, die mehrere Partikel verwaltet

Effektfactory und Presets:

EffectPresets: Liefert vorkonfigurierte ParticleEffectProperties (z.B. für Feuer, Rauch)
EffectFactory: Erstellt komplette Effekte mit zusätzlichen Konfigurationen wie Texturen und Farbpaletten
Datenfluss:

EffectPresets (vordefinierte Konfigurationen)
↓
EffectFactory (erstellt Effekte mit diesen Konfigurationen)
↓
ParticleEffect (verwaltet eine Gruppe von Partikeln)
↓
Particle (einzelne Partikel mit Position, Geschwindigkeit, Aussehen)

Sonderfälle:
Tracer: Ein spezieller Effekt, der statt von BaseEffect von MovingObject erbt
