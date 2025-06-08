// PowerupTypes.h
#pragma once

enum class PowerupType {
    None,
    GrowPaddle, // '1'
    ShrinkPaddle, // '2'
    Die, // '3'
    Glue, // '4'
    Multiball, // '5'
    GoThrough, // '6'
    Drop, // '7'
    Detonate, // '8'
    ExplosiveGrow, // '9'
    EasyBrick, // 'A'
    Explosive, // 'B'
    NextLevel, // 'C'
    AimHelp, // 'D'
    Coin, // 'E'
    BigBall, // 'F'
    NormalBall, // 'G'
    SmallBall, // 'H'
    Aim, // 'I'
    Life, // 'O'
    Gun, // 'P'
    Laser, // 'R'

    // Spezielle Random-Powerups
    Random10, // 'J' - 10% Chance
    Random100, // 'K' - 100% Chance
    Random5, // 'L' - 5% Chance
    Random2, // 'M' - 2% Chance
    Random1, // 'N' - 1% Chance
    RandomEvil // 'Q' - 100% böses PowerUp
};