# Save Game Manager

## Current Version (3.1.0)

Hier sind die Hauptprobleme, die ich sehe:

Enge Kopplung: MAXPOTEXTURES ist eine globale Konstante, die direkt in der Struktur verwendet wird. Das macht die Datenstruktur von externen Definitionen abhängig.

Boolsche Arrays für Powerups: Die Verwendung eines Arrays fester Größe für Powerups ist unflexibel. Beim Hinzufügen neuer Powerups müssen alle Speicherformate aktualisiert werden.

Gut, hier kam der Shop, das ist legitim
Inkonsistente Struktur-Versionen: player_struct_version_1 fehlt das coins-Feld, was zu Kompatibilitätsproblemen führt.

Fehlende Abstraktion: Die player_struct ist nur eine Datenstruktur ohne Verhalten oder Validierung.

Eine moderne Spieler-Klasse sollte:
- Daten kapseln und Validierung bieten
- Flexibel bei Powerup-Erweiterungen sein
- Serialisierung/Deserialisierung unterstützen
- Versionierung handhaben

Ich würde empfehlen, zuerst eine saubere Player-Klasse zu implementieren, die verschiedene Serialisierungsformate unterstützt (z.B. für alte Speicherstände). Danach kann der SaveGameManager überarbeitet werden, um diese Klasse zu nutzen, anstatt direkt mit den Strukturen zu arbeiten.

Zudem sollte PowerUp als eigene Klasse oder Enum implementiert werden, statt als Array von booleans mit magischen Indizes.

## Save game Version 2

Datenstrukturen
savedGame: Eine Struktur mit Namen (32 Zeichen) und Spielerattributen
player_struct: Enthält Spielerdaten wie Münzen, Level, Leben, Punktzahl und Powerups

saveGame(int slot, string name)
Speichert den aktuellen Spielstand in einem bestimmten Slot
Verwendet SOLPlayer (Start-of-Level-Spieler) für die Daten
Schreibt die Daten binär in die Speicherdatei an die Position des Slots

loadGame(int slot)
Lädt einen Spielstand aus einem Slot
Positioniert den Dateizeiger auf den richtigen Slot
Überträgt die geladenen Daten auf den aktiven Spieler
Dekrementiert das Level um 1, da nextlevel es später wieder inkrementiert

clearSaveGames()
Initialisiert eine neue Speicherdatei mit Version und leeren Slots
Schreibt zuerst den Header mit der Version
Erstellt sechs leere Slots mit dem Namen "Empty Slot"

listSaveGames(string slotName[])
Liest alle Spielstände aus der Datei
Prüft die Version der Speicherdatei
Falls die Version nicht übereinstimmt, werden alle Slots zurückgesetzt
Füllt das übergebene Array mit den Namen der Spielstände
Gibt die Anzahl der gelesenen Slots zurück

Dateiformat
Beginnt mit einem Header (Integer), der die Version enthält
Gefolgt von sechs savedGame-Strukturen
Binäres Format für effiziente Speicherung und Lesevorgänge
Der Code nutzt Binärdateien zur Verwaltung und überwacht die Versionskompatibilität, um sicherzustellen, dass ältere Speicherstände nicht mit neueren Versionen des Spiels inkompatibel werden.

```c++
#define SAVEGAME_VERSION 2

struct savedGame {
    char name[32];
    player_struct player;
};

void saveGame(int slot, const std::string &name, const player_struct &player) const;
bool loadGame(int slot, player_struct &player) const;
void clearSaveGames() const;
int listSaveGames(std::string slotNames[6]) const;

int listSaveGames(string slotName[]);
void loadGame(int slot);
void saveGame(int slot, string name);

struct player_struct player;
struct player_struct SOLPlayer;

void loadGame(int slot)
{
    fstream file;
    struct savedGame game;
    file.open(privFile.saveGameFile.data(), ios::in | ios::binary);
    if(!file.is_open())
        cout << "Could not open '" << privFile.saveGameFile << "' for Reading." << endl;
    
    //first, move to the slot
    file.seekg( sizeof(int)+(sizeof(savedGame)*slot) );
    file.read((char *)(&game), sizeof(savedGame));
    file.close();
    
    if(game.player.level != 0) //Only change level if level is not 0 (level one cannot be saved, and is used for the empty slots)
    {
    game.player.level--; //subtract one, because when the savegame is loaded, nextlevel is used to apply changes. nextlevel will add one to the level.
    player = game.player;
    gVar.nextlevel=1;
    var.paused=1;
    }
}

//Savegame files now consist of a int long header with a version number.
void saveGame(int slot, string name) {
fstream file;
struct savedGame game;
strcpy(game.name, name.data() );
game.player = SOLPlayer; //StartOfLevelPlayer, player as it was in the start of the level
file.open(privFile.saveGameFile.data(), ios::out | ios::in | ios::binary);
if(!file.is_open())
{
cout << "Could not open '"<<privFile.saveGameFile<<"' for Read+Write." << endl;
}

//move to the slot, mind the header
file.seekp( (sizeof(int))+((sizeof(savedGame)*slot)) );

file.write((char *)(&game), sizeof(savedGame));
file.close();
}

void clearSaveGames()
{
fstream file;
file.open(privFile.saveGameFile.data(), ios::out | ios::in | ios::binary);
if(!file.is_open())
{
cout << "Could not open '"<<privFile.saveGameFile<<"' for Read+Write." << endl;
}
//Write the header
const int sgHead = SAVEGAMEVERSION; //Savegame file version
file.write( (char *)(&sgHead), sizeof(int));
file.close();


saveGame(0, "Empty Slot");
saveGame(1, "Empty Slot");
saveGame(2, "Empty Slot");
saveGame(3, "Empty Slot");
saveGame(4, "Empty Slot");
saveGame(5, "Empty Slot");
}

int listSaveGames(string slotName[])
{
fstream file;
struct savedGame slot[6];
int i=0;

file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
if(!file.is_open())
{
cout << "Creating savegame slots in '"<<privFile.saveGameFile<<"'."<<endl;
file.open(privFile.saveGameFile.data(), ios::out | ios::binary);
if(!file.is_open())
{
cout << "Do not have permissions to write '" << privFile.saveGameFile << "'" <<endl;
return(0);
}
file.close();

    clearSaveGames();
    file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
    if(!file.is_open())
    {
      cout << "Could not write template."<<endl;
      return(0);
    }
}

    //First we check if this is the right version
int sgHead=0x00; //Invalid version

file.read((char *)(&sgHead), sizeof(int));
if(sgHead!=SAVEGAMEVERSION)
{
cout << "Savegame format error, is v" << sgHead << " should be v'"  << SAVEGAMEVERSION << "'." << endl;
cout << "Overwriting old savegames..." << endl;
file.close();
clearSaveGames();

    file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
    file.seekp(sizeof(int));
}

while(1)
{
if(i == 6)
{
break;
}

    file.read((char *)(&slot[i]), sizeof(savedGame));
    if( file.eof())
    {
      break;
    }

      slotName[i] = slot[i].name;
    i++;
}
file.close();


return(i);
}

struct player_struct {
int coins;
int multiply;
bool powerup[MAXPOTEXTURES];
bool explodePaddle; //This lock makes the paddle explode and it won't come back until newlife.
int level;
int lives;
int difficulty;
int score;
};
```

# Save Game Version 1

Hier sind die Hauptunterschiede zwischen der Version 1 des SaveGameManager und der aktuellen Version:

Strukturelle Unterschiede
Keine Versionsprüfung im Format Version 1

Version 1 hat keinen Header mit Versionsnummer
Version 2 beginnt mit einem 4-Byte-Integer für die Versionsnummer
Dateizugriffspositionen

Version 1: file.seekp(sizeof(savedGame) * slot)
Version 2: file.seekp(sizeof(int) + (sizeof(savedGame) * slot)) (verschoben durch Header)
Speicherzustand

Version 1: Speichert direkt den aktuellen player-Zustand
Version 2: Speichert SOLPlayer (Start-of-Level-Player)
Fehlerbehandlung

Version 2 verwendet SDL_Log statt cout für Fehlermeldungen
Version 2 gibt bei loadGame einen bool-Wert zurück
Implementierung

Version 1: Globale Funktionen mit globalen Variablen (player, privFile, gVar.nextlevel, var.paused)
Version 2: Methoden einer Klasse mit Parametern für Datenübergabe
clearSaveGames
Version 1: Ruft einfach saveGame für jeden Slot auf
Version 2: Schreibt zuerst den Header und ruft dann saveGame für jeden Slot auf

Dateiformatunterschied
Version 1:
[Slot 0 (savedGame)][Slot 1][Slot 2][Slot 3][Slot 4][Slot 5]

Version 2:
[Version (int)][Slot 0 (savedGame)][Slot 1][Slot 2][Slot 3][Slot 4][Slot 5]

```c++
struct savedGame {
char name[32];
struct player_struct player;
};

void saveGame(int slot, string name) {
fstream file;
struct savedGame game;
strcpy(game.name, name.data() );
game.player = player;
file.open(privFile.saveGameFile.data(), ios::out | ios::in | ios::binary);
if(!file.is_open())
{
cout << "Could not open '"<<privFile.saveGameFile<<"' for Read+Write." << endl;
}
//first, move to the slot
file.seekp( sizeof(savedGame)*slot );

file.write((char *)(&game), sizeof(savedGame));
file.close();
}

void clearSaveGames()
{
saveGame(0, "Empty Slot");
saveGame(1, "Empty Slot");
saveGame(2, "Empty Slot");
saveGame(3, "Empty Slot");
saveGame(4, "Empty Slot");
saveGame(5, "Empty Slot");
}

void loadGame(int slot)
{
fstream file;
struct savedGame game;
file.open(privFile.saveGameFile.data(), ios::in | ios::binary);
if(!file.is_open())
cout << "Could not open '" << privFile.saveGameFile << "' for Reading." << endl;

//first, move to the slot
file.seekg( sizeof(savedGame)*slot );
file.read((char *)(&game), sizeof(savedGame));
file.close();

if(game.player.level != 0) //Only change level if level is not 0 (level one cannot be saved, and is used for the empty slots)
{
game.player.level--; //subtract one, because when the savegame is loaded, nextlevel is used to apply changes. nextlevel will add one to the level.
player = game.player;
gVar.nextlevel=1;
var.paused=1;
}
}

int listSaveGames(string slotName[])
{
fstream file;
struct savedGame slot[6];
int i=0;

file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
if(!file.is_open())
{
cout << "Creating savegame slots in '"<<privFile.saveGameFile<<"'."<<endl;
file.open(privFile.saveGameFile.data(), ios::out | ios::binary);
if(!file.is_open())
{
cout << "Do not have permissions to write '" << privFile.saveGameFile << "'" <<endl;
return(0);
}
file.close();

    clearSaveGames();
    file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
    if(!file.is_open())
    {
      cout << "Could not write template."<<endl;
      return(0);
    }
}

while(1)
{
if(i == 6)
{
break;
}

    file.read((char *)(&slot[i]), sizeof(savedGame));
    if( file.eof())
    {
      break;
    }
      slotName[i] = slot[i].name;
    i++;
}
file.close();
return(i);
}
```