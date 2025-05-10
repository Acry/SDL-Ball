#pragma once
#include <GL/gl.h>
#include "GameObject.h"
#include <array>

// nasty fix to a problem
int nbrick[23][26];
int updated_nbrick[23][26];

/*
    Beispiel
    Brick::resetGrid(); // Grid initialisieren

    Brick brick;
    brick.setType('1');
    brick.updateGridPosition(42); // Setzt Position im Grid
*/

class Brick : public GameObject {
public:
    static constexpr int ROWS = 23;
    static constexpr int COLS = 26;

    Brick();

    // Getter/Setter
    void setScore(int value);
    [[nodiscard]] int getScore() const;
    void setDestroyToWin(bool value);
    [[nodiscard]] bool mustDestroyToWin() const;
    void setPowerup(char value);
    [[nodiscard]] char getPowerup() const;
    void setType(char value);
    [[nodiscard]] char getType() const;

    // Gameplay Methoden
    void hit();
    void makeExplosive();
    [[nodiscard]] bool hasNeighbor(int direction) const;
    void updateGridPosition(int gridIndex);

    static void resetGrid();
    static void updateGrid();

private:
    int score;
    bool destroytowin;
    char powerup;
    char type;
    GLfloat fade;
    GLfloat fadespeed;
    GLfloat zoomspeed;
    GLfloat zoom;
    bool isdyingnormally;
    bool isexploding;
    int row;
    int bricknum;
    int hitsLeft;
    bool justBecomeExplosive;

    static std::array<std::array<int, COLS>, ROWS> grid;
    static std::array<std::array<int, COLS>, ROWS> updatedGrid;
};


