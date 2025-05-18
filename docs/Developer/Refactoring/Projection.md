# Projection

Die alte Projektion verwendete:

gluPerspective(45.0f, ratio, 0.1f, 10.0f) - Eine perspektivische Projektion mit 45° FOV
Die neue Projektion verwendet:

glOrtho(-1, 1, -1, 1, -1, 1) - Eine orthogonale Projektion im Einheitswürfel

Die Umrechnung der Koordinaten erfolgt nach der Formel:
neuerWert = alterWert * (tan(FOV/2) * |Z-Position|) / 2.0

Mit den konkreten Werten:

FOV = 45°
Z-Position = 3.0 (typische Z-Distanz in der perspektivischen Ansicht)
Ergibt sich der Skalierungsfaktor:
Faktor = (tan(45°/2) * 3.0) / 2.0 ≈ 0.27

Die orthogonale Projektion ist für ein 2D-Spiel wie SDL-Ball in der Regel die bessere Wahl.
