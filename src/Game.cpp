#include "Game.hpp"
#include <cmath>
#include <vector>

Game::Game()
    : window(sf::VideoMode(1080, 660), "8-Ball Pool Arena - Proyek DPBO (C++ & SFML)", sf::Style::Titlebar | sf::Style::Close),
      state(GameState::MainMenu),
      fontLoaded(false),
      table(130.f, 125.f, 860.f, 430.f),
      menu(1080.f, 660.f),
      turnInProgress(false),
      cueBallStartPos(130.f + 860.f * 0.25f, 125.f + 430.f * 0.5f) {
    window.setFramerateLimit(60);
    loadSystemFont();
    initBalls();
}

void Game::loadSystemFont() {
    std::vector<std::string> fontPaths = {
        "assets/fonts/font.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
}

void Game::initBalls() {
    balls.clear();
    float r = 14.f;

    // 1. Bola Putih (ID = 0)
    balls.emplace_back(0, cueBallStartPos.x, cueBallStartPos.y, sf::Color(248, 250, 252), r);

    // 2. 10 Bola Target (ID = 1..10)
    std::vector<sf::Color> palette = {
        sf::Color(234, 179, 8),   // 1: Kuning
        sf::Color(37, 99, 235),   // 2: Biru
        sf::Color(220, 38, 38),   // 3: Merah
        sf::Color(126, 34, 206),  // 4: Ungu
        sf::Color(234, 88, 12),   // 5: Oranye
        sf::Color(22, 163, 74),   // 6: Hijau
        sf::Color(153, 27, 27),   // 7: Maroon
        sf::Color(23, 23, 23),    // 8: Hitam
        sf::Color(234, 179, 8),   // 9: Kuning Stripe
        sf::Color(37, 99, 235)    // 10: Biru Stripe
    };

    float apexX = table.bounds.left + table.bounds.width * 0.70f;
    float apexY = table.bounds.top + table.bounds.height * 0.5f;
    float colSpacing = r * 1.76f;
    float rowSpacing = r * 2.04f;

    int idCounter = 1;
    for (int col = 0; col < 4; ++col) {
        int numInCol = col + 1;
        float x = apexX + col * colSpacing;
        float startY = apexY - (numInCol - 1) * (rowSpacing / 2.f);
        for (int row = 0; row < numInCol; ++row) {
            float y = startY + row * rowSpacing;
            balls.emplace_back(idCounter, x, y, palette[(idCounter - 1) % palette.size()], r);
            idCounter++;
        }
    }
}

void Game::resetMatch() {
    scoreBoard.reset();
    initBalls();
    turnInProgress = false;
    state = GameState::Playing;
}

bool Game::areAllBallsStopped() const {
    for (const auto& b : balls) {
        if (b.isMoving()) return false;
    }
    return true;
}

int Game::countActiveTargetBalls() const {
    int count = 0;
    for (size_t i = 1; i < balls.size(); ++i) {
        if (balls[i].active) count++;
    }
    return count;
}

void Game::placeCueBallSafely() {
    sf::Vector2f candidate = cueBallStartPos;
    float minClearance = balls[0].radius * 2.2f;

    for (int offsetStep = 0; offsetStep < 15; ++offsetStep) {
        bool blocked = false;
        for (size_t i = 1; i < balls.size(); ++i) {
            if (!balls[i].active) continue;
            sf::Vector2f otherPos = balls[i].shape.getPosition();
            if (std::hypot(candidate.x - otherPos.x, candidate.y - otherPos.y) < minClearance) {
                blocked = true;
                break;
            }
        }
        if (!blocked) break;
        candidate.y = cueBallStartPos.y + ((offsetStep % 2 == 0) ? 1.f : -1.f) * ((offsetStep / 2 + 1) * 28.f);
    }
    balls[0].reset(candidate.x, candidate.y);
}

void Game::handleEvents() {
    sf::Event event;
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        MenuAction action = menu.handleEvent(event, mousePos, state == GameState::MainMenu);
        if (action == MenuAction::StartGame || action == MenuAction::RestartGame) {
            resetMatch();
            continue;
        } else if (action == MenuAction::ExitGame) {
            window.close();
            return;
        }

        if (state == GameState::Playing) {
            bool canShoot = !turnInProgress && areAllBallsStopped();
            cue.handleEvent(event, mousePos, balls[0], canShoot);
        }
    }
}

void Game::update() {
    if (state != GameState::Playing) return;

    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
    bool canShoot = !turnInProgress && areAllBallsStopped();

    if (cue.update(mousePos, balls[0], balls, table, canShoot)) {
        turnInProgress = true;
    }

    // Sub-stepping Fisika 4x per frame
    const int SUBSTEPS = 4;
    float stepRatio = 1.f / static_cast<float>(SUBSTEPS);

    for (int step = 0; step < SUBSTEPS; ++step) {
        for (auto& b : balls) {
            if (!b.active) continue;
            b.update(stepRatio);
            table.handleWallCollision(b);

            if (table.isBallPotted(b)) {
                b.active = false;
                b.velocity = {0.f, 0.f};
                scoreBoard.onBallPotted(b.id, b.baseColor);
            }
        }
        table.resolveBallCollisions(balls);
    }

    if (turnInProgress && areAllBallsStopped()) {
        turnInProgress = false;

        if (!balls[0].active) {
            placeCueBallSafely();
        }

        scoreBoard.evaluateTurnEnd();

        if (countActiveTargetBalls() == 0) {
            state = GameState::GameOver;
        }
    }
}

void Game::render() {
    window.clear(sf::Color(15, 23, 42));

    table.draw(window);
    for (const auto& b : balls) {
        b.draw(window, &font, fontLoaded);
    }

    if (state == GameState::MainMenu) {
        menu.drawMainMenu(window, &font, fontLoaded);
    } else {
        scoreBoard.drawHUD(window, &font, fontLoaded);
        menu.drawInGameButtons(window, &font, fontLoaded);

        bool canShoot = (state == GameState::Playing) && !turnInProgress && areAllBallsStopped();
        cue.draw(window, balls[0], canShoot, &font, fontLoaded);

        if (state == GameState::GameOver) {
            menu.drawGameOverModal(window, &font, fontLoaded, scoreBoard.scoreP1, scoreBoard.scoreP2);
        }
    }

    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}