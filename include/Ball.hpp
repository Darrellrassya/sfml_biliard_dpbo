#pragma once
#include <SFML/Graphics.hpp>

class Ball {
public:
    int id; // 0 = Bola Putih, 1..8 = Solid, 9..10 = Stripe
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float radius;
    bool active;
    sf::Color baseColor;

    Ball(int ballId, float x, float y, sf::Color color, float r = 14.f);

    void update(float subStepRatio = 1.f);
    bool isMoving() const;
    void reset(float x, float y);
    void draw(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const;
};