#pragma once
#include <vector>
#include <random>

#include "Powerup.h"
#include "EventManager.h"
#include "CollisionManager.h"

//enum class PowerupType {
//    Aim,            // Zielhilfe
//    AimHelp,        // Zielhilfe-Hilfe
//    BigBall,        // Großer Ball
//    Bullet,         // Schuss
//    Coin,           // Münze
//    Detonate,       // Detonator
//    Die,            // Leben verlieren
//    Drop,           // Drop Powerup
//    EasyBrick,      // Einfacher Brick
//    Explosive,      // Explosiver Brick
//    ExplosiveGrow,  // Explosiver Wachstum
//    Glue,           // Kleber Powerup
//    GoThrough,      // Durchgehen Powerup
//    GrowPaddle,     // Paddle wachsen lassen
//    Gun,            // Waffe Powerup
//    Laser,          // Laser Powerup
//    Life,           // Extra Leben
//    Multiball,      // Mehrere Bälle
//};

class PowerupManager {
    std::vector<Powerup> powerups;
    EventManager *eventManager;
    CollisionManager *collisionManager;

    std::mt19937 rng;
    std::uniform_int_distribution<int> powerupDist;
    std::uniform_real_distribution<float> dropChanceDist;

    int maxPowerups;
    float dropChance;

public:
    PowerupManager(EventManager *eventMgr, CollisionManager *collMgr, int maxPowerupsCount = 5);

    ~PowerupManager() = default;

    void init();

    void update(float deltaTime);

    void draw(float deltaTime);

    void spawnPowerup(float posX, float posY);

    void trySpawnPowerup(float posX, float posY);

    void setDropChance(float chance) { dropChance = std::min(1.0f, std::max(0.0f, chance)); }
    [[nodiscard]] float getDropChance() const { return dropChance; }
};

/*
 *class powerupLoaderClass {
public:
    //most common
    std::string chances[4];
    int powerups;
    int powerupsGiven;
    std::string evilPowerups;

    powerupLoaderClass() {
        int chance = -1;
        powerups = 0;
        powerupsGiven = 0;
        std::string line;

        // Which powerups are evil?
        evilPowerups = "2H37"; //P

        std::ifstream powerFile(themeManager.getThemeFilePath("/powerups.txt", setting.lvlTheme).data());
        if (!powerFile.is_open()) {
            SDL_Log(" Could not open 'powerups.txt'");
            var.quit = true;
            return;
        }

        while (!powerFile.eof()) {
            getline(powerFile, line);
            if (line.compare("Most:") == 0) {
                chance = 0;
            } else if (line.compare("More:") == 0) {
                chance = 1;
            } else if (line.compare("Less:") == 0) {
                chance = 2;
            } else if (line.compare("Least:") == 0) {
                chance = 3;
            } else {
                //No matches, either its a comment or a powerup.
                if (line.compare(0, 2, "//") != 0 && line.compare("") != 0) {
                    chances[chance] += line[0];
                    powerups++;
                }
            }
        }
    }

    char selectRandomPowerup() {
        int i = rand() % 100;
        powerupsGiven++;
        int group = (i < 1) ? 3 : (i < 11) ? 2 : (i < 41) ? 1 : 0;
        i = rand() % chances[group].length();
        return chances[group][i];
    }

    char randomPowerup(const char p) {
        const int i = rand() % 100;
        switch (p) {
            case 'Q': // 100% evil
                return randomEvilPowerup();
            case 'K': // 100%
                return selectRandomPowerup();
            case 'N': // 1%
                if (i < 1) return selectRandomPowerup();
                return '0';
            case 'M': // 2%
                if (i < 2) return selectRandomPowerup();
                return '0';
            case 'L': // 5%
                if (i < 5) return selectRandomPowerup();
                return '0';
            case 'J': // 10%
                if (i < 10) return selectRandomPowerup();
                return '0';
            default:
                return p;
        }
    }

    char randomEvilPowerup() const {
        const int i = rand() % evilPowerups.length();
        return evilPowerups[i];
    }
};
*/