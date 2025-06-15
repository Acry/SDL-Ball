## Level-Daten exportieren

```c++
void exportLevelData() const {
    std::string filename = "level_" + std::to_string(ctx.currentLevel) + "_data.txt";
    std::ofstream file(filename);
    
    if (file.is_open()) {
        file << "Level " << ctx.currentLevel << " Daten:\n";
        file << "Anzahl Bricks: " << ctx.bricks.size() << "\n\n";
        
        for (size_t i = 0; i < ctx.bricks.size(); i++) {
            const auto& brick = ctx.bricks[i];
            file << "Brick " << i << ":\n";
            file << "  Typ: " << static_cast<int>(brick.type) << "\n";
            file << "  Position: (" << brick.x << ", " << brick.y << ")\n";
            file << "  Custom Color: (" << 
                static_cast<int>(brick.CustomBrickColor.r) << ", " <<
                static_cast<int>(brick.CustomBrickColor.g) << ", " <<
                static_cast<int>(brick.CustomBrickColor.b) << ", " <<
                static_cast<int>(brick.CustomBrickColor.a) << ")\n\n";
        }
        
        ctx.textManager.addAnnouncement("Level-Daten exportiert: " + filename, 2000, Fonts::AnnounceGood);
    }
}
```

## Grafische Layout-Analyse

```c++
void analyzeLayout() const {
    // Berechne die Belegungsdichte des Spielfelds
    float totalArea = 2.0f * 2.0f; // Gesamtfläche (-1 bis 1 in x und y)
    float brickArea = 0.0f;
    
    for (const auto& brick : ctx.bricks) {
        brickArea += BRICK_WIDTH * BRICK_HEIGHT;
    }
    
    float coverage = (brickArea / totalArea) * 100.0f;
    
    char info[64];
    sprintf(info, "Flächenabdeckung: %.1f%%", coverage);
    ctx.textManager.write(info, Fonts::Normal, true, 1.0f, 0.0f, -0.8f);
}
```
