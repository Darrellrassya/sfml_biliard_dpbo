#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"
#include "Table.hpp"

struct AimRayResult {
    bool hitBall = false;
    int hitBallId = -1;
    sf::Vector2f ghostBallPos;
    sf::Vector2f targetBallPos;
    sf::Vector2f targetMoveDir; // Garis arah pantulan untuk 1 bola target pertama
    sf::Vector2f cueDeflectDir; // Garis arah belok bola putih setelah tumbukan
    float targetLineLen = 0.f;
    float cueDeflectLen = 0.f;
};

class Cue {
public:
    bool isDragging;
    bool draggingFromMeter;
    bool isStriking;
    float strikeProgress;

    float power;
    float maxPower;
    float pullDistance;
    float maxPullDistance;

    sf::Vector2f aimDirection;
    sf::Vector2f dragStartMouse;
    AimRayResult aimResult;
    sf::FloatRect powerMeterRect;

    Cue();

    bool handleEvent(const sf::Event& event, const sf::Vector2f& mousePos, Ball& cueBall, bool canShoot);
    bool update(const sf::Vector2f& mousePos, Ball& cueBall, const std::vector<Ball>& balls, const Table& table, bool canShoot);
    void computeAimRaycast(const Ball& cueBall, const std::vector<Ball>& balls, const Table& table);
    void draw(sf::RenderWindow& window, const Ball& cueBall, bool canShoot, const sf::Font* font, bool fontLoaded) const;
};