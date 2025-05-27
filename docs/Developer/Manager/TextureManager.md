# TextureManager

Ich denke, wir sollten zuerst die Properties auslesen und dann die Textur laden.
Denn überlicherweise steht dort die Datei, die geladen werden soll.

[ ] Wenn ein Textur-Theme nicht in ordnung ist, default-Thema nutzen
[ ] Embedded Thema

    const std::filesystem::path texturePath = "../themes/default/gfx/paddle/base.png";
    const std::filesystem::path propsPath = "../themes/default/gfx/paddle/base.txt";

Machen wir nicht. Wir lassen die Endung weg. Dann wird versucht beides zu laden.

Test für brick/glass.png und brick/glass.txt

file=brick/glass.png
ticks=1000
cols=1
rows=2
xoffset=1.0
yoffset=0.5
frames=2
playing=0
bidir=0
padding=0
texcolor=FFFFFFA0
parcolor=FFFFFF

Test für brick/green.txt

Fehler: Keine Bilddatei für...
Ein Fehler ist es erst dann, wenn file=brick/base.png a) weder bild noch textur, b) im text die property file= fehlt
oder leer ist. oder c) die Datei nicht existiert.

## Bisher:

Explizites Laden von Texturen und SpriteSheetAnimationen

```c++
"SpriteSheetAnimation.h"
"TextureProperties.h"

TextureManager textureManager
SpriteSheetAnimation tex;

Ball ball(&eventManager);
const std::filesystem::path ballTexPath = "../themes/default/gfx/ball/normal.png";
const std::filesystem::path ballPropsPath = "../themes/default/gfx/ball/normal.txt";

if (!textureManager.load(ballTexPath, ball.texture)) {
    SDL_Log("Fehler beim Laden der Ball-Textur: %s", ballTexPath.c_str());
    return EXIT_FAILURE;
}
textureManager.readTexProps(ballPropsPath, ball.texture);
```

## Verknüpfung ThemeManager - TextureManager

GameManager::loadResources() sollte die Texturen und Animationen laden, die vom ThemeManager definiert sind.
Der ThemeManager erhält die Pfade zu den Themen vom SettingsManager.

Der TextureManager lädt die Texturen zu laden und die SpriteSheetAnimationen zu erstellen.

Der ThemeManager muss aufgebohrt werden. snd (Audio, Graphics, Level, Fonts, Powerups, Backgrounds, Title (Scenes?)).

Hiergeht es nur um die Texturen und Animationen, die in den Theme-Dateien definiert sind.
Aktuell:

gfx/
├── ball
├── brick
├── effects
├── highscore
├── menu
├── paddle
└── powerup

sollten von Ball, nach Effects verschoben werden?
├── tail.png
└── tail.txt

Der ThemeManager sollte unterscheiden zwischen kompletten Themes, TeilThemen und inkrementellen Änderungen.
Den TextureManager interessieren nur die Texturen und die Properties, die in den Theme-Dateien definiert sind.

Also: setSpriteTheme()
/home/carsten/sources/extern/GameDev/example_games/breakout/SDL-Ball/SDL-Ball-Project/themes/default

gfx/
├── ball
├── brick
├── effects
├── highscore
├── menu
├── paddle
└── powerup

nach name.
Aktuell wird SDL2 IMG_Load genutzt.

Wo findet extern DECLSPEC int SDLCALL IMG_Init(int flags); statt?
SDL_SetWindowIcon(display.sdlWindow,
IMG_Load(themeManager.getThemeFilePath("icon32.png", setting.gfxTheme).data()));
Display, oder TextureManager?

`IMG_INIT_JPG`
`IMG_INIT_PNG`
`IMG_INIT_TIF`
`IMG_INIT_WEBP`
`IMG_INIT_JXL`
`IMG_INIT_AVIF`

Wer ruft extern DECLSPEC void SDLCALL IMG_Quit(void); auf?

Nun müssen wir irgendwo eine Liste der benötigten Texturen führen, die geladen werden müssen.
Die .txt (später json) Dateien enthalten die Eigenschaften der Texturen, die geladen werden müssen.
Diese können automatisch geladen werden, wenn die Textur geladen wird.

GameObject haben die Eigenschaft: SpriteSheetAnimation texture;
den Typ sollte ich auch so nennen, damit es klar ist, dass es sich um eine Animation handelt.

Die Animation ist optional. aber auch das image ist optional.
Entweder eine name.txt, oder eine name + endung der unterstützen , oder beides.

siehe: brick/purple.txt:
file=brick/base.png
ticks=1000
cols=1
rows=1
xoffset=1.0
yoffset=1.0
frames=0
playing=0
padding=1
texcolor=FF00FFFF
parcolor=FF00FF
file=brick/base.png

Verwendung im Spiel:

// Im GameManager

```c++
void GameManager::initialize() {
    // Theme-Einrichtung
    themeManager.initialize(configFileManager.getConfigDir() + "/themes");
    textureManager.setThemeManager(&themeManager);
    textureManager.setTheme(settings.gfxTheme);

    // Texturen werden automatisch geladen
    
    // Zugriff auf Texturen im Code
    ball.setTexture(textureManager.getBallTexture(BallTexture::Normal));
    paddle.setBaseTexture(textureManager.getPaddleTexture(PaddleTexture::Base));
    
    // Schnelles Umschalten bei Power-Ups
    if (isPowerUpActive(PowerUpType::Fireball)) {
        ball.setTexture(textureManager.getBallTexture(BallTexture::Fireball));
    }
}

void GameManager::changeTheme(const std::string& themeName) {
    settings.gfxTheme = themeName;
    textureManager.setTheme(themeName);
    
    // Aktualisiere alle Spielobjekte mit den neuen Texturen
    updateGameObjectTextures();
}

void GameManager::updateGameObjectTextures() {
    // Alle Spielobjekte mit neuen Texturen aktualisieren
    paddle.setBaseTexture(textureManager.getPaddleTexture(PaddleTexture::Base));
    // usw.
}
```

## Verknüpfung SpriteSheetAnimation - TextureManager

```c++
// Hinzufügen zu TextureManager.h
SpriteSheetAnimation getSpriteSheetAnimation(const std::string& name) const;

// Implementierung in TextureManager.cpp
SpriteSheetAnimation TextureManager::getSpriteSheetAnimation(const std::string& name) const {
    SpriteSheetAnimation result;
    const std::filesystem::path texPath = "../themes/default/gfx/" + name + ".png";
    const std::filesystem::path propsPath = "../themes/default/gfx/" + name + ".txt";
    
    if (!load(texPath, result)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texPath.c_str());
    } else {
        readTexProps(propsPath, result);
    }
    
    return result;
}
```
