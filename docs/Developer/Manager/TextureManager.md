# TextureManager

## Konzept

Ablauf:

SETTING:
TextureManager::setTheme - highest level, loads all themes

    TextureManager::setSpriteTheme
        TextureManager::setBallTheme
        TextureManager::setPaddleTheme
        TextureManager::setBrickTheme
    TextureManager::setPowerupTheme
    TextureManager::setBgTheme
    TextureManager::setEffectsTheme
    TextureManager::setMiscTheme
    TextureManager::setHudTheme
    TextureManager::setMenuTheme

LOADING:
TextureManager::loadAllGameTextures -> TextureManager::loadAndCacheTexture -> TextureManager::loadTextureWithProperties

GETTING:
getAllTextures
getBallTexture
getPaddleTexture
aso.
getTexture

## Thoughts

Klare Typisierung mit spezialisierten Texturklassen
Homogenisierung des Textur-Systems mit automatischer Zuweisung
Homogenes Rendering-Interface über die virtuelle draw-Methode

ball brick und paddle mal an, die erben von Gameobject und haben GameObject:: SpriteSheetAnimation texture;

Ball hat zum beidpiel noch zusätzlich SpriteSheetAnimation fireTex;

Paddle SpriteSheetAnimation *layerTex{};

ich würde das ergene homogenisieren.

SpriteSheetAnimation fireTex; würde der typ SpriteSheetAnimation fireball heißen, wie im theme wäre die zuordnung
leicht.
sie ersetzt die bestehende textur.

bei den layern ist das anders, die werden über der z oder unter der z tiefe gezeichnet
der glue-layer, hat eine höhrere z tiefe.

der gun layer, sogar einen höheren Y-wert. also eine andere art von layer

wie wäre es mit typen die vom textur-typ erben.

Texturhierarchie
z.B. Texture -> SpritsheetAnimation -> ExtenderSpritsheetAnimation
-> LayeredSpriteSheetAnimation
-> ReplaceSpriteSheetAnimation

class StaticTexture : public Texture
class SpriteTexture : public Texture
class LayeredTexture : public SpriteTexture
class SwitchableTexture : public SpriteTexture

setTexture<LayeredTexture>();
auto* layeredTex = dynamic_cast<LayeredTexture*>(texture.get());
if (layeredTex) {
// Glue-Layer hinzufügen
SpriteTexture glueTex;
// glueTex konfigurieren...
layeredTex->addLayer(glueTex);
layeredTex->setLayerActive(0, false); // Standardmäßig deaktiviert

void setGlueLayer(bool enabled) {
hasGlueLayer = enabled;
auto* layeredTex = dynamic_cast<LayeredTexture*>(texture.get());
if (layeredTex) {
layeredTex->setLayerActive(0, enabled);
}
}
void setGunLayer(bool enabled) {
hasGunLayer = enabled;
auto* layeredTex = dynamic_cast<LayeredTexture*>(texture.get());
if (layeredTex) {
layeredTex->setLayerActive(1, enabled);
}
}
void draw(float deltaTime) override {
if (!active) return;

        // Vereinfachte Zeichenroutine
        texture->draw(pos_x, pos_y, width, height, deltaTime);
    }

## Cache rework

Das Problem liegt darin, dass loadTextureWithProperties direkt vom Code aufgerufen wird, ohne den Cache zu prüfen.

Eine Lösung besteht darin, alle Texturladungen über loadAndCacheTexture zu leiten.

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
