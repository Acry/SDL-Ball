# TextureManager

"Texture.h"
"texture_properties.h"

verknüpfen themeManager texture Manager

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
