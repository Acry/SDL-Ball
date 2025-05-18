# Kollisionserkennung

Die Kollisionserkennung sollte in der GameObject-Basisklasse implementiert sein, damit alle abgeleiteten Klassen (Ball, Brick, Paddle) sie nutzen können.

Das Paddle selbst sollte keine Kollisionen prüfen müssen - stattdessen:
Die zentrale Spiellogik sollte prüfen, ob der Ball mit dem Paddle kollidiert
Die Kollisionserkennung mit den Bildschirmrändern sollte im update()-Teil des Paddle-Codes stattfinden, wie du es bereits implementiert hast:

