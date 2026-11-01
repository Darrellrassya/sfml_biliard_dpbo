#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class ScoreBoard {
public:
    int scoreP1;
    int scoreP2;
    int currentPlayer;
    int pottedTargetCountThisTurn;
    bool cueBallFoulThisTurn;
    std::string infoBanner;
    std::vector<sf::Color> pottedColorsP1;
    std::vector<sf::Color> pottedColorsP2;

    ScoreBoard();

    void reset();
    void onBallPotted(int ballId, sf::Color ballColor);
    void evaluateTurnEnd();
    void drawHUD(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const;
};