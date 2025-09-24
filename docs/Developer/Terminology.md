Terminologie-Vorschlag:

Punktesystem (Score System):
Das Regelwerk und die Mechanik, wie und wofür der Spieler Punkte erhält. Dies ist für den Spieler sichtbar und
beschreibt, wie Punkte im Spiel verdient werden können (z.B. durch das Zerstören von Bricks, Powerups,
Multiplikator-Regeln).

Punkteprozess (Score Process):
Der technische Ablauf im Code, wie Punkte intern verarbeitet, akkumuliert und Events ausgelöst werden. Hierzu zählen die
beteiligten Klassen (Manager, Compositor), Events und deren Zusammenspiel.

Manager:
Verwaltet und steuert einen bestimmten Aspekt des Spiels (z.B. BrickManager, PlayerManager). Verantwortlich für Logik
und Datenhaltung.

Compositor:
Kümmert sich um die Darstellung und das Zusammenführen von Informationen für die Anzeige (z.B. HudCompositor).

Empfehlung für die Dokumentation:
Ein eigenes Kapitel „Terminologie“ am Anfang, das diese Begriffe klar definiert.

Im weiteren Verlauf immer klar zwischen „Punktesystem“ (Spielmechanik, Concept) und „Punkteprozess“ (technischer Ablauf,
Flow)
unterscheiden.

## Pattern und naming

Es gibt keine fest definierte Menge an Suffixen für klassische Design Patterns, aber es haben sich einige Konventionen
etabliert.

Häufig verwendete Suffixe sind:
Manager: Für verwaltende, koordinierende Klassen (z.\ B. ResourceManager, EventManager).
Factory: Für Erzeuger-Pattern (z.\ B. WidgetFactory).
Builder: Für das Builder-Pattern (z.\ B. CarBuilder).
Controller: Für Steuerung/Logik (z.\ B. InputController).
Strategy: Für austauschbare Algorithmen (z.\ B. SortStrategy).
Observer: Für Beobachter (z.\ B. ScoreObserver).
Subject: Für das beobachtete Objekt (z.\ B. EventSubject).
Decorator: Für das Decorator-Pattern (z.\ B. LoggingDecorator).
Mediator: Für das Mediator-Pattern (z.\ B. DialogMediator).
Command: Für das Command-Pattern (z.\ B. SaveCommand).
Adapter: Für das Adapter-Pattern (z.\ B. LegacyAdapter).
Proxy: Für das Proxy-Pattern (z.\ B. ImageProxy).
Compositor: Wird selten als Pattern-Suffix verwendet, aber für Kompositions- oder Darstellungsobjekte (z.\ B.
HudCompositor).
