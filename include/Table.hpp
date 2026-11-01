#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"

class Table {
public:
    sf::FloatRect bounds;
    sf::RectangleShape outerFrame;
    sf::RectangleShape woodRail;
    sf::RectangleShape cushionBorder;
    sf::RectangleShape feltArea;
    std::vector<sf::CircleShape> pockets;
    std::vector<sf::CircleShape> diamondSights;
    float pocketRadius;

    Table(float x, float y, float width, float height);

    void handleWallCollision(Ball& ball) const;
    bool isBallPotted(const Ball& ball) const;
    void resolveBallCollisions(std::vector<Ball>& balls) const;
    void draw(sf::RenderWindow& window) const;
};