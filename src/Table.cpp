#include "Table.hpp"
#include <cmath>

Table::Table(float x, float y, float width, float height)
    : bounds(x, y, width, height), pocketRadius(24.f) {
    float cushionW = 14.f;
    float railW = 36.f;

    outerFrame.setSize({width + (railW + 6.f) * 2.f, height + (railW + 6.f) * 2.f});
    outerFrame.setPosition(x - railW - 6.f, y - railW - 6.f);
    outerFrame.setFillColor(sf::Color(45, 20, 10));
    outerFrame.setOutlineThickness(3.f);
    outerFrame.setOutlineColor(sf::Color(217, 119, 6));

    woodRail.setSize({width + railW * 2.f, height + railW * 2.f});
    woodRail.setPosition(x - railW, y - railW);
    woodRail.setFillColor(sf::Color(92, 40, 18));
    woodRail.setOutlineThickness(2.f);
    woodRail.setOutlineColor(sf::Color(28, 12, 6));

    cushionBorder.setSize({width + cushionW * 2.f, height + cushionW * 2.f});
    cushionBorder.setPosition(x - cushionW, y - cushionW);
    cushionBorder.setFillColor(sf::Color(6, 95, 70));

    feltArea.setSize({width, height});
    feltArea.setPosition(x, y);
    feltArea.setFillColor(sf::Color(16, 150, 105));

    // 6 Lubang Meja
    std::vector<sf::Vector2f> coords = {
        {x - 3.f, y - 3.f},
        {x + width / 2.f, y - 7.f},
        {x + width + 3.f, y - 3.f},
        {x - 3.f, y + height + 3.f},
        {x + width / 2.f, y + height + 7.f},
        {x + width + 3.f, y + height + 3.f}
    };

    for (const auto& pt : coords) {
        sf::CircleShape p(pocketRadius);
        p.setOrigin(pocketRadius, pocketRadius);
        p.setPosition(pt);
        p.setFillColor(sf::Color(10, 12, 16));
        p.setOutlineThickness(4.f);
        p.setOutlineColor(sf::Color(217, 119, 6));
        pockets.push_back(p);
    }

    // 18 Titik Diamond Sights di pinggiran kayu meja
    auto addDiamond = [&](float dx, float dy) {
        sf::CircleShape d(3.5f, 4);
        d.setOrigin(3.5f, 3.5f);
        d.setPosition(dx, dy);
        d.setFillColor(sf::Color(253, 230, 138));
        diamondSights.push_back(d);
    };

    float halfW = width / 2.f;
    for (int i = 1; i <= 3; ++i) {
        float offset = (halfW / 4.f) * i;
        addDiamond(x + offset, y - railW * 0.52f);
        addDiamond(x + halfW + offset, y - railW * 0.52f);
        addDiamond(x + offset, y + height + railW * 0.52f);
        addDiamond(x + halfW + offset, y + height + railW * 0.52f);
    }
    for (int i = 1; i <= 3; ++i) {
        float offsetY = (height / 4.f) * i;
        addDiamond(x - railW * 0.52f, y + offsetY);
        addDiamond(x + width + railW * 0.52f, y + offsetY);
    }
}

void Table::handleWallCollision(Ball& ball) const {
    if (!ball.active) return;

    sf::Vector2f pos = ball.shape.getPosition();

    // Jangan pantulkan ke dinding jika bola sudah berada di mulut lubang
    for (const auto& pocket : pockets) {
        sf::Vector2f pPos = pocket.getPosition();
        if (std::hypot(pos.x - pPos.x, pos.y - pPos.y) < pocketRadius * 1.28f) {
            return;
        }
    }

    float r = ball.radius;
    if (pos.x - r < bounds.left) {
        ball.shape.setPosition(bounds.left + r, pos.y);
        ball.velocity.x = -ball.velocity.x * 0.92f;
    } else if (pos.x + r > bounds.left + bounds.width) {
        ball.shape.setPosition(bounds.left + bounds.width - r, pos.y);
        ball.velocity.x = -ball.velocity.x * 0.92f;
    }

    pos = ball.shape.getPosition();
    if (pos.y - r < bounds.top) {
        ball.shape.setPosition(pos.x, bounds.top + r);
        ball.velocity.y = -ball.velocity.y * 0.92f;
    } else if (pos.y + r > bounds.top + bounds.height) {
        ball.shape.setPosition(pos.x, bounds.top + bounds.height - r);
        ball.velocity.y = -ball.velocity.y * 0.92f;
    }
}

bool Table::isBallPotted(const Ball& ball) const {
    if (!ball.active) return false;
    sf::Vector2f bPos = ball.shape.getPosition();
    for (const auto& pocket : pockets) {
        sf::Vector2f pPos = pocket.getPosition();
        if (std::hypot(bPos.x - pPos.x, bPos.y - pPos.y) < pocketRadius - 1.f) {
            return true;
        }
    }
    return false;
}

void Table::resolveBallCollisions(std::vector<Ball>& balls) const {
    for (size_t i = 0; i < balls.size(); ++i) {
        if (!balls[i].active) continue;
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (!balls[j].active) continue;

            sf::Vector2f posA = balls[i].shape.getPosition();
            sf::Vector2f posB = balls[j].shape.getPosition();
            sf::Vector2f delta = posB - posA;
            float dist = std::hypot(delta.x, delta.y);
            float minDist = balls[i].radius + balls[j].radius;

            if (dist < minDist && dist > 0.0001f) {
                float overlap = 0.5f * (minDist - dist);
                sf::Vector2f normal = delta / dist;
                balls[i].shape.move(-normal * overlap);
                balls[j].shape.move(normal * overlap);

                sf::Vector2f relVel = balls[i].velocity - balls[j].velocity;
                float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

                if (velAlongNormal > 0.f) {
                    sf::Vector2f impulse = normal * velAlongNormal;
                    balls[i].velocity -= impulse;
                    balls[j].velocity += impulse;
                }
            }
        }
    }
}

void Table::draw(sf::RenderWindow& window) const {
    sf::RectangleShape shadow = outerFrame;
    shadow.move(0.f, 10.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 140));
    shadow.setOutlineThickness(0.f);
    window.draw(shadow);

    window.draw(outerFrame);
    window.draw(woodRail);
    window.draw(cushionBorder);
    window.draw(feltArea);

    float baulkX = bounds.left + bounds.width * 0.25f;
    sf::Vertex line[] = {
        sf::Vertex({baulkX, bounds.top}, sf::Color(255, 255, 255, 85)),
        sf::Vertex({baulkX, bounds.top + bounds.height}, sf::Color(255, 255, 255, 85))
    };
    window.draw(line, 2, sf::Lines);

    sf::CircleShape spot(3.f);
    spot.setOrigin(3.f, 3.f);
    spot.setFillColor(sf::Color(255, 255, 255, 110));
    spot.setPosition(baulkX, bounds.top + bounds.height * 0.5f);
    window.draw(spot);
    spot.setPosition(bounds.left + bounds.width * 0.70f, bounds.top + bounds.height * 0.5f);
    window.draw(spot);

    for (const auto& d : diamondSights) {
        window.draw(d);
    }
    for (const auto& pocket : pockets) {
        window.draw(pocket);
    }
}