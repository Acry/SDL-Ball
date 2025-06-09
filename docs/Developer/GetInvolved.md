# Get involved

I want to keep this project as close as possbile to the original game, but I also want to refactor it to make it more
maintainable and easier to work with.
To understand what is going on, one should start understanding test-background.
Ich versuche die Struktur und von zwei Seiten her anzugehen. Bottom-up, um die Basis zu verstehen und top-down, um die
Struktur zu verbessern.
Je nachdem, wie es um den Stand steht, arbeite ich an der Dokumentation oder an der Code-Qualität.

I highly doubt anyone will pick this up; nowadays nobody likes to do things without payment.
It is a fun project and a nice game, the refactoring is challenging.
Don't want to end this up as yet another broken 2D game engine.

If you are kinda experienced, you can refactor the [managers](Managers.md).
I started to write tests for the managers, [Refactoring](Todo.md##Refactoring).

The game will be unplayable for a while, most importantly is that `make tests` works, before commiting.
Those are not programmable tests, but they are a good start.

For smaller Todos check out [Todo](Todo.md). You need a running branch for that.

Thank you, Jimmy Christensens for writing this game and sharing it with the world.

## Todo

Auch die Todo-Struktur ist nicht ganz einfach.
Es gibt die Liste Todo.md, die viele Todos enthält, die ich mir so notiert habe.
Geraten Topics out of hand, entsteht für ein Theme eine eigene Datei.
[Todo](Todo.md)

[Current](CURRENT.md) ist eine Art Statusbericht, was gerade so läuft, oder was ich gerade so mache, bzw. mich
beschäftigt.

Wie gesagt, nicht jeder Code läuft, bei jedem Commit. Momentan sind die manuellen Tests die Art wie der Code getrieben
wird.
Desktop UI's und Games sind nicht so einfach zu testen, wie command line tools, libraries oder statische/dynamische
Webseiten.
Dinge wie pupeteer, selenium, playwright sind im open-source bereich nicht vorhanden.

Für die automatischen Tests, möchte ich gtest nutzen. Je enger die Kopplung, desto schwieriger gestaltet sich das.

Zwischenziele sind also Interfaces und dependency injection, um die Kopplung zu lockern.
Danach ist das mocken von Klassen einfacher und die Tests können geschrieben, bzw. lauffähig werden.

## Interesting Checkouts

SDL1 - Retro Branch
`b66514f3b08667dbc4da76e8e0a8de3ea209ba6b`

Stable commit before most changes - you need SDL-Ball and SDL-Ball-Data from a repository.
`658e263364412b71fa9495389470b126103b4409`

Last building commit before refactoring of manager classes
`1c37e8e93355bcb99dc235bcb40cab234229b7ec`
