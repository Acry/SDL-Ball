# EffectManager und EventManager

Vorteile dieses Ansatzes:
Lose Kopplung: Ball und Tracer kommunizieren indirekt über Events
Saubere Trennung: Jede Komponente hat eine klare Verantwortung
Erweiterbar: Neue Effekte können leicht auf dieselben Events reagieren
Wartbar: Ball muss nicht wissen, wie Tracer implementiert ist
Flexibel: Tracer kann auf Events anderer Objekte reagieren
Diese Architektur erlaubt eine klare Trennung von Zuständigkeiten:


EventManager: Kommunikation zwischen Komponenten
EffectManager: Darstellung und Verwaltung von Partikeln

Ball: Spiellogik ohne Detailwissen über visuelle Effekte
Tracer: Visuelle Darstellung ohne Abhängigkeit von Spiellogik

Dieser Ansatz folgt dem `Publish-Subscribe-Pattern` und dem Prinzip der `Inversion of Control`, was **für komplexere Spiele empfehlenswert** ist.
