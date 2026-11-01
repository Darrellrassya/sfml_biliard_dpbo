#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Table.hpp"
#include "Ball.hpp"
#include "Cue.hpp"
#include "ScoreBoard.hpp"
#include "Menu.hpp"

enum class GameState {
    MainMenu,
    Playing,
    GameOver
};

class Game {
private:
    sf::RenderWindow window;
    GameState state;
    sf::Font font;
    bool fontLoaded;

    Table table;
    std::vector<Ball> balls;
    Cue cue;
    ScoreBoard scoreBoard;
    Menu menu;

    bool turnInProgress;
    sf::Vector2f cueBallStartPos;

    void loadSystemFont();
    void initBalls();
    bool areAllBallsStopped() const;
    int countActiveTargetBalls() const;
    void placeCueBallSafely();

public:
    Game();
    void run();
    void handleEvents();
    void update();
    void render();
    void resetMatch();
};