#include "ScoreBoard.hpp"

ScoreBoard::ScoreBoard() {
    reset();
}

void ScoreBoard::reset() {
    scoreP1 = 0;
    scoreP2 = 0;
    currentPlayer = 1;
    pottedTargetCountThisTurn = 0;
    cueBallFoulThisTurn = false;
    pottedColorsP1.clear();
    pottedColorsP2.clear();
    infoBanner = "GILIRAN PEMAIN 1";
}

void ScoreBoard::onBallPotted(int ballId, sf::Color ballColor) {
    if (ballId == 0) {
        cueBallFoulThisTurn = true;
    } else {
        pottedTargetCountThisTurn++;
        if (currentPlayer == 1) {
            scoreP1++;
            pottedColorsP1.push_back(ballColor);
        } else {
            scoreP2++;
            pottedColorsP2.push_back(ballColor);
        }
    }
}

void ScoreBoard::evaluateTurnEnd() {
    if (cueBallFoulThisTurn) {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        infoBanner = "FOUL! BOLA PUTIH MASUK - GILIRAN PEMAIN " + std::to_string(currentPlayer);
    } else if (pottedTargetCountThisTurn > 0) {
        infoBanner = "BOLA MASUK! PEMAIN " + std::to_string(currentPlayer) + " LANJUT MEMUKUL";
    } else {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        infoBanner = "GILIRAN PEMAIN " + std::to_string(currentPlayer);
    }

    pottedTargetCountThisTurn = 0;
    cueBallFoulThisTurn = false;
}

void ScoreBoard::drawHUD(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const {
    auto drawPlayerPanel = [&](float x, float y, bool activeTurn, const std::string& name,
                               int score, const std::vector<sf::Color>& pottedList, bool alignRight) {
        sf::RectangleShape panel({290.f, 58.f});
        panel.setPosition(x, y);
        panel.setFillColor(sf::Color(30, 41, 59));
        panel.setOutlineThickness(activeTurn ? 2.5f : 1.5f);
        panel.setOutlineColor(activeTurn ? sf::Color(56, 189, 248) : sf::Color(51, 65, 85));
        window.draw(panel);

        sf::RectangleShape scoreBadge({44.f, 44.f});
        float badgeX = alignRight ? (x + 7.f) : (x + 290.f - 51.f);
        scoreBadge.setPosition(badgeX, y + 7.f);
        scoreBadge.setFillColor(activeTurn ? sf::Color(56, 189, 248) : sf::Color(51, 65, 85));
        window.draw(scoreBadge);

        if (fontLoaded && font) {
            sf::Text nameTxt;
            nameTxt.setFont(*font);
            nameTxt.setString(name);
            nameTxt.setCharacterSize(15);
            nameTxt.setStyle(sf::Text::Bold);
            nameTxt.setFillColor(sf::Color(248, 250, 252));
            nameTxt.setPosition(alignRight ? (x + 64.f) : (x + 14.f), y + 7.f);
            window.draw(nameTxt);

            sf::Text scTxt;
            scTxt.setFont(*font);
            scTxt.setString(std::to_string(score));
            scTxt.setCharacterSize(20);
            scTxt.setStyle(sf::Text::Bold);
            scTxt.setFillColor(activeTurn ? sf::Color(15, 23, 42) : sf::Color::White);
            sf::FloatRect sb = scTxt.getLocalBounds();
            scTxt.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
            scTxt.setPosition(badgeX + 22.f, y + 29.f);
            window.draw(scTxt);
        }

        float slotStartX = alignRight ? (x + 66.f) : (x + 16.f);
        for (int i = 0; i < 6; ++i) {
            sf::CircleShape slot(7.f);
            slot.setOrigin(7.f, 7.f);
            slot.setPosition(slotStartX + i * 20.f + 7.f, y + 41.f);
            if (i < static_cast<int>(pottedList.size())) {
                slot.setFillColor(pottedList[i]);
                slot.setOutlineThickness(1.f);
                slot.setOutlineColor(sf::Color::White);
            } else {
                slot.setFillColor(sf::Color(15, 23, 42));
                slot.setOutlineThickness(1.f);
                slot.setOutlineColor(sf::Color(71, 85, 105));
            }
            window.draw(slot);
        }
    };

    drawPlayerPanel(85.f, 14.f, currentPlayer == 1, "PEMAIN 1", scoreP1, pottedColorsP1, false);
    drawPlayerPanel(515.f, 14.f, currentPlayer == 2, "PEMAIN 2", scoreP2, pottedColorsP2, true);

    sf::CircleShape vsBadge(20.f);
    vsBadge.setOrigin(20.f, 20.f);
    vsBadge.setPosition(445.f, 43.f);
    vsBadge.setFillColor(sf::Color(250, 204, 21));
    vsBadge.setOutlineThickness(2.f);
    vsBadge.setOutlineColor(sf::Color(15, 23, 42));
    window.draw(vsBadge);

    if (fontLoaded && font) {
        sf::Text vsTxt;
        vsTxt.setFont(*font);
        vsTxt.setString("VS");
        vsTxt.setCharacterSize(14);
        vsTxt.setStyle(sf::Text::Bold);
        vsTxt.setFillColor(sf::Color(15, 23, 42));
        sf::FloatRect vb = vsTxt.getLocalBounds();
        vsTxt.setOrigin(vb.left + vb.width / 2.f, vb.top + vb.height / 2.f);
        vsTxt.setPosition(445.f, 43.f);
        window.draw(vsTxt);

        sf::Text banner;
        banner.setFont(*font);
        banner.setString(infoBanner);
        banner.setCharacterSize(14);
        banner.setStyle(sf::Text::Bold);
        banner.setFillColor(sf::Color(56, 189, 248));
        sf::FloatRect bb = banner.getLocalBounds();
        banner.setOrigin(bb.left + bb.width / 2.f, 0.f);
        banner.setPosition(535.f, 590.f);
        window.draw(banner);
    }
}