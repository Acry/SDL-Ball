# Idiotlock

Der Entwickler hat in der README-Datei selbstironisch angemerkt, dass er beim Schreiben von SDL-Ball viel gelernt hat
und viele Dinge „falsch“ gemacht wurden, weil er es damals nicht besser wusste. Er schreibt ausdrücklich, dass man das
Projekt nicht als Beispiel für gutes Spieldesign nehmen sollte, ist aber trotzdem stolz darauf, dass es funktioniert.
Das ist eindeutig selbstironisch und reflektiert ein Bewusstsein für die eigenen Fehler.

idiotlock ist eine Variable, die im Hauptspiel-Loop verwendet wird, um zu verhindern, dass bestimmte Aktionen mehrfach
ausgeführt werden, wenn ein Level abgeschlossen wird. Sie sorgt dafür, dass der Code zur Level-Übergabe (z.B.
Punktevergabe, Levelwechsel, Powerup-Reset) nur einmal pro Level-Abschluss ausgeführt wird, auch wenn die Bedingungen
mehrfach erfüllt sind. Das verhindert unbeabsichtigte Mehrfachausführungen durch z.B. Timing-Effekte oder Schleifen.