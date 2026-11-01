#include "Ball.hpp"
#include <cmath>
#include <string>

Ball::Ball(int ballId, float x, float y, sf::Color color, float r)
    : id(ballId), velocity(0.f, 0.f), radius(r), active(true), baseColor(color) {
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(x, y);
    shape.setFillColor(color);
    shape.setOutlineThickness(1.5f);
    shape.setOutlineColor(sf::Color(15, 23, 42, 220));
}

void Ball::update(float subStepRatio) {
    if (!active) return;

    shape.move(velocity * subStepRatio);

    float friction = std::pow(0.9865f, subStepRatio);
    velocity *= friction;

    if (std::hypot(velocity.x, velocity.y) < 0.065f) {
        velocity = {0.f, 0.f};
    }
}

bool Ball::isMoving() const {
    return active && (velocity.x != 0.f || velocity.y != 0.f);
}

void Ball::reset(float x, float y) {
    shape.setPosition(x, y);
    velocity = {0.f, 0.f};
    active = true;
}

void Ball::draw(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const {
    if (!active) return;

    sf::Vector2f pos = shape.getPosition();

    // 1. Bayangan lembut di atas karpet meja
    sf::CircleShape shadow(radius * 1.02f);
    shadow.setOrigin(radius * 1.02f, radius * 1.02f);
    shadow.setPosition(pos.x + 3.5f, pos.y + 4.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 115));
    window.draw(shadow);

    // 2. Lingkaran utama bola
    window.draw(shape);

    // 3. Ring putih untuk bola Stripe (nomor 9 & 10)
    if (id >= 9) {
        sf::CircleShape stripeRing(radius * 0.82f);
        stripeRing.setOrigin(radius * 0.82f, radius * 0.82f);
        stripeRing.setPosition(pos);
        stripeRing.setFillColor(sf::Color::Transparent);
        stripeRing.setOutlineThickness(2.2f);
        stripeRing.setOutlineColor(sf::Color(248, 250, 252, 220));
        window.draw(stripeRing);
    }

    // 4. Nomor bola di tengah (untuk bola target id > 0)
    if (id > 0) {
        sf::CircleShape badge(radius * 0.50f);
        badge.setOrigin(radius * 0.50f, radius * 0.50f);
        badge.setPosition(pos);
        badge.setFillColor(sf::Color(248, 250, 252));
        window.draw(badge);

        if (fontLoaded && font) {
            sf::Text numTxt;
            numTxt.setFont(*font);
            numTxt.setString(std::to_string(id));
            numTxt.setCharacterSize(10);
            numTxt.setStyle(sf::Text::Bold);
            numTxt.setFillColor(sf::Color(15, 23, 42));
            sf::FloatRect b = numTxt.getLocalBounds();
            numTxt.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
            numTxt.setPosition(pos);
            window.draw(numTxt);
        }
    } else {
        // Titik merah kecil pada bola putih ala 8 Ball Pool
        sf::CircleShape redDot(2.2f);
        redDot.setOrigin(2.2f, 2.2f);
        redDot.setPosition(pos.x + 2.f, pos.y - 1.f);
        redDot.setFillColor(sf::Color(220, 38, 38, 180));
        window.draw(redDot);
    }

    // 5. Efek kilau 3D (Specular Highlight)
    sf::CircleShape gloss(radius * 0.28f);
    gloss.setOrigin(radius * 0.28f, radius * 0.28f);
    gloss.setPosition(pos.x - radius * 0.35f, pos.y - radius * 0.35f);
    gloss.setFillColor(sf::Color(255, 255, 255, 110));
    window.draw(gloss);
}