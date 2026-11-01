#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class MenuAction {
    None,
    StartGame,
    RestartGame,
    ExitGame
};

class Menu {
private:
    float winWidth;
    float winHeight;
    sf::RectangleShape btnStart;
    sf::RectangleShape btnExitMain;
    sf::RectangleShape btnRestartTop;
    sf::RectangleShape btnExitTop;

public:
    Menu(float width, float height);

    MenuAction handleEvent(const sf::Event& event, const sf::Vector2f& mousePos, bool inMainMenu);
    void drawMainMenu(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const;
    void drawInGameButtons(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const;
    void drawGameOverModal(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded, int s1, int s2) const;
};