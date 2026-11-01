#include "Menu.hpp"

Menu::Menu(float width, float height) : winWidth(width), winHeight(height) {
    btnStart.setSize({270.f, 54.f});
    btnStart.setPosition(width / 2.f - 135.f, height / 2.f - 5.f);

    btnExitMain.setSize({270.f, 54.f});
    btnExitMain.setPosition(width / 2.f - 135.f, height / 2.f + 68.f);

    btnRestartTop.setSize({105.f, 42.f});
    btnRestartTop.setPosition(830.f, 22.f);

    btnExitTop.setSize({105.f, 42.f});
    btnExitTop.setPosition(945.f, 22.f);
}

MenuAction Menu::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos, bool inMainMenu) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R && !inMainMenu) return MenuAction::RestartGame;
        if (event.key.code == sf::Keyboard::Escape) return MenuAction::ExitGame;
        if (event.key.code == sf::Keyboard::Enter && inMainMenu) return MenuAction::StartGame;
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (inMainMenu) {
            if (btnStart.getGlobalBounds().contains(mousePos)) return MenuAction::StartGame;
            if (btnExitMain.getGlobalBounds().contains(mousePos)) return MenuAction::ExitGame;
        } else {
            if (btnRestartTop.getGlobalBounds().contains(mousePos)) return MenuAction::RestartGame;
            if (btnExitTop.getGlobalBounds().contains(mousePos)) return MenuAction::ExitGame;
        }
    }
    return MenuAction::None;
}

static void drawSleekBtn(sf::RenderWindow& window, const sf::RectangleShape& rect, sf::Color bgCol,
                         sf::Color txtCol, const std::string& label, const sf::Font* font, bool fontLoaded, unsigned int fSize = 15) {
    sf::RectangleShape sh = rect;
    sh.move(0.f, 4.f);
    sh.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(sh);

    sf::RectangleShape b = rect;
    b.setFillColor(bgCol);
    b.setOutlineThickness(2.f);
    b.setOutlineColor(sf::Color(255, 255, 255, 50));
    window.draw(b);

    if (fontLoaded && font) {
        sf::Text t;
        t.setFont(*font);
        t.setString(label);
        t.setCharacterSize(fSize);
        t.setStyle(sf::Text::Bold);
        t.setFillColor(txtCol);
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        t.setPosition(rect.getPosition().x + rect.getSize().x / 2.f,
                      rect.getPosition().y + rect.getSize().y / 2.f);
        window.draw(t);
    }
}

void Menu::drawMainMenu(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const {
    sf::RectangleShape overlay({winWidth, winHeight});
    overlay.setFillColor(sf::Color(11, 15, 25, 200));
    window.draw(overlay);

    sf::RectangleShape card({600.f, 360.f});
    card.setPosition(winWidth / 2.f - 300.f, winHeight / 2.f - 180.f);
    card.setFillColor(sf::Color(30, 41, 59));
    card.setOutlineThickness(3.f);
    card.setOutlineColor(sf::Color(250, 204, 21));
    window.draw(card);

    if (fontLoaded && font) {
        sf::Text title;
        title.setFont(*font);
        title.setString("billiard game");
        title.setCharacterSize(36);
        title.setStyle(sf::Text::Bold);
        title.setFillColor(sf::Color(250, 204, 21));
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition(winWidth / 2.f, winHeight / 2.f - 115.f);
        window.draw(title);

        sf::Text sub;
        sub.setFont(*font);
        sub.setString("C++17 & SFML  |  Darrell  ,  Aqil  ,  Raihan");
        sub.setCharacterSize(14);
        sub.setFillColor(sf::Color(148, 163, 184));
        sf::FloatRect sb = sub.getLocalBounds();
        sub.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
        sub.setPosition(winWidth / 2.f, winHeight / 2.f - 65.f);
        window.draw(sub);
    }

    drawSleekBtn(window, btnStart, sf::Color(34, 197, 94), sf::Color::White, "MAIN SEKARANG", font, fontLoaded, 17);
    drawSleekBtn(window, btnExitMain, sf::Color(239, 68, 68), sf::Color::White, "KELUAR GAME", font, fontLoaded, 16);
}

void Menu::drawInGameButtons(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded) const {
    drawSleekBtn(window, btnRestartTop, sf::Color(234, 179, 8), sf::Color(15, 23, 42), "ULANG (R)", font, fontLoaded, 13);
    drawSleekBtn(window, btnExitTop, sf::Color(239, 68, 68), sf::Color::White, "KELUAR", font, fontLoaded, 13);
}

void Menu::drawGameOverModal(sf::RenderWindow& window, const sf::Font* font, bool fontLoaded, int s1, int s2) const {
    sf::RectangleShape overlay({winWidth, winHeight});
    overlay.setFillColor(sf::Color(11, 15, 25, 180));
    window.draw(overlay);

    sf::RectangleShape modal({520.f, 220.f});
    modal.setPosition(winWidth / 2.f - 260.f, winHeight / 2.f - 110.f);
    std::string winText = "PERTANDINGAN SERI!";
    if (s1 > s2) winText = "PEMAIN 1 MENANG! (" + std::to_string(s1) + " - " + std::to_string(s2) + ")";
    else if (s2 > s1) winText = "PEMAIN 2 MENANG! (" + std::to_string(s2) + " - " + std::to_string(s1) + ")";

    drawSleekBtn(window, modal, sf::Color(30, 41, 59), sf::Color(250, 204, 21),
                 winText + "\n\nTekan 'R' atau Klik 'ULANG' untuk Main Lagi", font, fontLoaded, 18);
}