#pragma once
#include "game_state.h"

// GameState.h
enum class GameState {
    TITLE,
    PLAYING,
    PAUSED,
    MENU,
    GAME_OVER,
    HIGH_SCORE,
    LEVEL_COMPLETE,
    CALIBRATING
};

class GameStateMachine {
    GameState currentState;
    GameState previousState;

public:
    GameStateMachine() : currentState(GameState::TITLE), previousState(GameState::TITLE) {}

    void setState(const GameState newState) {
        previousState = currentState;
        currentState = newState;
        onStateEnter(newState);
    }

    void returnToPreviousState() {
        setState(previousState);
    }

    GameState getState() const {
        return currentState;
    }

    bool isInState(const GameState state) const {
        return currentState == state;
    }

private:
    static void onStateEnter(const GameState state) {
        switch(state) {
            case GameState::TITLE:
                var.titleScreenShow = true;
                pauseGame();
                break;

            case GameState::PLAYING:
                var.titleScreenShow = false;
                var.menu = 0;
                resumeGame();
                break;

            case GameState::PAUSED:
                pauseGame();
                break;

            case GameState::MENU:
                var.menu = 1;
                pauseGame();
                break;

            case GameState::GAME_OVER:
                gVar.gameOver = true;
                pauseGame();
                break;

            case GameState::HIGH_SCORE:
                var.showHighScores = true;
                break;

            case GameState::LEVEL_COMPLETE:
                gVar.nextlevel = true;
                var.paused = true;
                break;

            case GameState::CALIBRATING:
                var.menu = 10;
                break;
        }
    }
};
