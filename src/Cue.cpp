#include "Cue.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

Cue::Cue()
    : isDragging(false),
      draggingFromMeter(false),
      isStriking(false),
      strikeProgress(0.f),
      power(0.f),
      maxPower(24.f),
      pullDistance(0.f),
      maxPullDistance(125.f),
      aimDirection(1.f, 0.f),
      dragStartMouse(0.f, 0.f),
      powerMeterRect(24.f, 155.f, 36.f, 380.f) {}

bool Cue::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos, Ball& cueBall, bool canShoot) {
    if (!canShoot || !cueBall.active || isStriking) {
        isDragging = false;
        return false;
    }

    // Fine-tuning sudut bidik pakai tombol Panah Kiri / Kanan di keyboard
    if (event.type == sf::Event::KeyPressed && !isDragging) {
        float angle = std::atan2(aimDirection.y, aimDirection.x);
        if (event.key.code == sf::Keyboard::Left) {
            angle -= 0.015f;
            aimDirection = {std::cos(angle), std::sin(angle)};
        } else if (event.key.code == sf::Keyboard::Right) {
            angle += 0.015f;
            aimDirection = {std::cos(angle), std::sin(angle)};
        }
    }

    // Klik Kiri: Kunci sudut bidikan & mulai tarik mundur
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (mousePos.y > 82.f) {
            isDragging = true;
            draggingFromMeter = powerMeterRect.contains(mousePos);
            dragStartMouse = mousePos;
            pullDistance = 0.f;
            power = 0.f;
        }
    }

    // Klik Kanan saat menarik = Batalkan tembakan
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && isDragging) {
        isDragging = false;
        pullDistance = 0.f;
        power = 0.f;
    }

    // Lepas Klik Kiri: Jalankan animasi sodokan stik jika tarikan cukup
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && isDragging) {
        isDragging = false;
        if (pullDistance >= 6.f && power >= 0.8f) {
            isStriking = true;
            strikeProgress = 0.f;
        } else {
            pullDistance = 0.f;
            power = 0.f;
        }
    }
    return false;
}

bool Cue::update(const sf::Vector2f& mousePos, Ball& cueBall, const std::vector<Ball>& balls, const Table& table, bool canShoot) {
    if (!canShoot || !cueBall.active) return false;

    // Animasi stik maju menyentuh bola putih sebelum bola meluncur
    if (isStriking) {
        strikeProgress += 0.25f;
        if (strikeProgress >= 1.f) {
            isStriking = false;
            strikeProgress = 0.f;
            cueBall.velocity = aimDirection * power;
            power = 0.f;
            pullDistance = 0.f;
            return true;
        }
        return false;
    }

    sf::Vector2f ballPos = cueBall.shape.getPosition();

    if (!isDragging) {
        if (!powerMeterRect.contains(mousePos) && mousePos.y > 82.f) {
            sf::Vector2f diff = mousePos - ballPos;
            float len = std::hypot(diff.x, diff.y);
            if (len > 2.f) {
                aimDirection = diff / len;
            }
        }
    } else {
        // Sudut aimDirection terkunci saat ditarik agar bidikan tidak goyang
        sf::Vector2f dragVec = mousePos - dragStartMouse;
        float pullAlongStick = -(dragVec.x * aimDirection.x + dragVec.y * aimDirection.y);
        float pullVertical = draggingFromMeter ? (mousePos.y - dragStartMouse.y) * 0.45f : 0.f;

        float rawPull = std::max(pullAlongStick * 0.65f, pullVertical);
        pullDistance = std::clamp(rawPull, 0.f, maxPullDistance);

        float ratio = pullDistance / maxPullDistance;
        power = std::pow(ratio, 1.25f) * maxPower;
    }

    computeAimRaycast(cueBall, balls, table);
    return false;
}

void Cue::computeAimRaycast(const Ball& cueBall, const std::vector<Ball>& balls, const Table& table) {
    sf::Vector2f p0 = cueBall.shape.getPosition();
    sf::Vector2f dir = aimDirection;
    float R = cueBall.radius;

    // 1. Cari jarak ke tepi meja terdekat
    float tWallX = std::numeric_limits<float>::max();
    float tWallY = std::numeric_limits<float>::max();

    if (dir.x > 0.0001f) tWallX = (table.bounds.left + table.bounds.width - R - p0.x) / dir.x;
    else if (dir.x < -0.0001f) tWallX = (table.bounds.left + R - p0.x) / dir.x;

    if (dir.y > 0.0001f) tWallY = (table.bounds.top + table.bounds.height - R - p0.y) / dir.y;
    else if (dir.y < -0.0001f) tWallY = (table.bounds.top + R - p0.y) / dir.y;

    float minT = std::max(0.f, std::min(tWallX, tWallY));
    int hitIdx = -1;

    // 2. Circle-Sweep Raycast: Cari 1 bola target PERTAMA yang tertabrak
    for (size_t i = 1; i < balls.size(); ++i) {
        if (!balls[i].active) continue;

        sf::Vector2f c = balls[i].shape.getPosition();
        sf::Vector2f toBall = c - p0;
        float proj = toBall.x * dir.x + toBall.y * dir.y;

        if (proj <= 0.f) continue;

        float distSq = toBall.x * toBall.x + toBall.y * toBall.y;
        float perpSq = distSq - proj * proj;
        float sumR = R + balls[i].radius;

        if (perpSq < sumR * sumR) {
            float offset = std::sqrt(sumR * sumR - perpSq);
            float tHit = proj - offset;
            if (tHit > 0.f && tHit < minT) {
                minT = tHit;
                hitIdx = static_cast<int>(i);
            }
        }
    }

    aimResult.ghostBallPos = p0 + dir * minT;
    aimResult.hitBall = (hitIdx != -1);

    if (aimResult.hitBall) {
        // Garis bantu pantulan hanya dihitung untuk 1 bola pertama ini saja
        aimResult.hitBallId = balls[hitIdx].id;
        aimResult.targetBallPos = balls[hitIdx].shape.getPosition();

        sf::Vector2f n = aimResult.targetBallPos - aimResult.ghostBallPos;
        float nLen = std::hypot(n.x, n.y);
        if (nLen > 0.0001f) n /= nLen;
        aimResult.targetMoveDir = n;

        float alignment = std::clamp(dir.x * n.x + dir.y * n.y, 0.f, 1.f);
        aimResult.targetLineLen = 45.f + 95.f * alignment;

        sf::Vector2f tangent = dir - n * (dir.x * n.x + dir.y * n.y);
        float tLen = std::hypot(tangent.x, tangent.y);
        if (tLen > 0.05f) {
            aimResult.cueDeflectDir = tangent / tLen;
            aimResult.cueDeflectLen = 30.f + 65.f * tLen;
        } else {
            aimResult.cueDeflectDir = {0.f, 0.f};
            aimResult.cueDeflectLen = 0.f;
        }
    } else {
        aimResult.hitBallId = -1;
        sf::Vector2f reflectDir = dir;
        if (tWallX < tWallY) reflectDir.x = -reflectDir.x;
        else reflectDir.y = -reflectDir.y;
        aimResult.cueDeflectDir = reflectDir;
        aimResult.cueDeflectLen = 45.f;
    }
}

void Cue::draw(sf::RenderWindow& window, const Ball& cueBall, bool canShoot, const sf::Font* font, bool fontLoaded) const {
    if (!canShoot || !cueBall.active) return;

    sf::Vector2f ballPos = cueBall.shape.getPosition();
    sf::Vector2f ghostPos = aimResult.ghostBallPos;

    // 1. Garis Bidik Utama
    sf::Vertex mainRay[] = {
        sf::Vertex(ballPos, sf::Color(255, 255, 255, 230)),
        sf::Vertex(ghostPos, sf::Color(255, 255, 255, 190))
    };
    window.draw(mainRay, 2, sf::Lines);

    // 2. Lingkaran Ghost Ball
    sf::CircleShape ghost(cueBall.radius);
    ghost.setOrigin(cueBall.radius, cueBall.radius);
    ghost.setPosition(ghostPos);
    ghost.setFillColor(sf::Color(255, 255, 255, 35));
    ghost.setOutlineThickness(1.8f);
    ghost.setOutlineColor(sf::Color(255, 255, 255, 220));
    window.draw(ghost);

    sf::CircleShape ghostCenter(2.5f);
    ghostCenter.setOrigin(2.5f, 2.5f);
    ghostCenter.setPosition(ghostPos);
    ghostCenter.setFillColor(sf::Color::White);
    window.draw(ghostCenter);

    // 3. Garis Bantu Pantulan (Hanya untuk 1 Bola yang Dikenai)
    if (aimResult.hitBall) {
        sf::Vector2f targetEnd = aimResult.targetBallPos + aimResult.targetMoveDir * aimResult.targetLineLen;
        sf::Vertex targetRay[] = {
            sf::Vertex(aimResult.targetBallPos, sf::Color(250, 204, 21, 245)),
            sf::Vertex(targetEnd, sf::Color(250, 204, 21, 90))
        };
        window.draw(targetRay, 2, sf::Lines);

        sf::CircleShape targetTip(3.f);
        targetTip.setOrigin(3.f, 3.f);
        targetTip.setPosition(targetEnd);
        targetTip.setFillColor(sf::Color(250, 204, 21));
        window.draw(targetTip);

        if (aimResult.cueDeflectLen > 1.f) {
            sf::Vector2f deflectEnd = ghostPos + aimResult.cueDeflectDir * aimResult.cueDeflectLen;
            sf::Vertex deflectRay[] = {
                sf::Vertex(ghostPos, sf::Color(255, 255, 255, 220)),
                sf::Vertex(deflectEnd, sf::Color(255, 255, 255, 70))
            };
            window.draw(deflectRay, 2, sf::Lines);
        }
    } else if (aimResult.cueDeflectLen > 1.f) {
        sf::Vector2f wallBounceEnd = ghostPos + aimResult.cueDeflectDir * aimResult.cueDeflectLen;
        sf::Vertex wallRay[] = {
            sf::Vertex(ghostPos, sf::Color(255, 255, 255, 160)),
            sf::Vertex(wallBounceEnd, sf::Color(255, 255, 255, 30))
        };
        window.draw(wallRay, 2, sf::Lines);
    }

    // 4. Visual Stik Biliar Multi-Segmen
    float angleDeg = std::atan2(aimDirection.y, aimDirection.x) * 180.f / 3.14159265f;
    float currentOffset = cueBall.radius + 10.f + (isStriking ? pullDistance * (1.f - strikeProgress) : pullDistance);

    sf::RectangleShape shaft({115.f, 5.5f});
    shaft.setOrigin(115.f + currentOffset, 2.75f);
    shaft.setPosition(ballPos);
    shaft.setRotation(angleDeg);
    shaft.setFillColor(sf::Color(245, 222, 179));
    shaft.setOutlineThickness(1.f);
    shaft.setOutlineColor(sf::Color(15, 23, 42));
    window.draw(shaft);

    sf::RectangleShape butt({95.f, 7.5f});
    butt.setOrigin(95.f + 115.f + currentOffset, 3.75f);
    butt.setPosition(ballPos);
    butt.setRotation(angleDeg);
    butt.setFillColor(sf::Color(120, 40, 15));
    butt.setOutlineThickness(1.2f);
    butt.setOutlineColor(sf::Color(15, 23, 42));
    window.draw(butt);

    sf::RectangleShape tip({6.f, 5.5f});
    tip.setOrigin(6.f + currentOffset, 2.75f);
    tip.setPosition(ballPos);
    tip.setRotation(angleDeg);
    tip.setFillColor(sf::Color(56, 189, 248));
    window.draw(tip);

    // 5. Vertical Power Meter di Sisi Kiri Layar
    sf::RectangleShape meterBg({powerMeterRect.width, powerMeterRect.height});
    meterBg.setPosition(powerMeterRect.left, powerMeterRect.top);
    meterBg.setFillColor(sf::Color(30, 41, 59));
    meterBg.setOutlineThickness(2.5f);
    meterBg.setOutlineColor(isDragging ? sf::Color(250, 204, 21) : sf::Color(71, 85, 105));
    window.draw(meterBg);

    float pullRatio = pullDistance / maxPullDistance;
    if (pullRatio > 0.01f) {
        float fillH = (powerMeterRect.height - 6.f) * pullRatio;
        sf::RectangleShape fill({powerMeterRect.width - 6.f, fillH});
        fill.setPosition(powerMeterRect.left + 3.f, powerMeterRect.top + 3.f);

        sf::Color pCol = sf::Color(74, 222, 128);
        if (pullRatio > 0.7f) pCol = sf::Color(244, 63, 94);
        else if (pullRatio > 0.35f) pCol = sf::Color(250, 204, 21);

        fill.setFillColor(pCol);
        window.draw(fill);
    }

    sf::RectangleShape miniCue({8.f, 120.f});
    miniCue.setOrigin(4.f, 0.f);
    miniCue.setPosition(powerMeterRect.left + powerMeterRect.width / 2.f,
                        powerMeterRect.top + 15.f + pullRatio * (powerMeterRect.height - 145.f));
    miniCue.setFillColor(sf::Color(245, 222, 179));
    miniCue.setOutlineThickness(1.f);
    miniCue.setOutlineColor(sf::Color::Black);
    window.draw(miniCue);

    if (fontLoaded && font) {
        sf::Text pwrTxt;
        pwrTxt.setFont(*font);
        int pct = static_cast<int>(std::round((power / maxPower) * 100.f));
        pwrTxt.setString(std::to_string(pct) + "%");
        pwrTxt.setCharacterSize(13);
        pwrTxt.setStyle(sf::Text::Bold);
        pwrTxt.setFillColor(sf::Color(248, 250, 252));
        sf::FloatRect b = pwrTxt.getLocalBounds();
        pwrTxt.setOrigin(b.left + b.width / 2.f, 0.f);
        pwrTxt.setPosition(powerMeterRect.left + powerMeterRect.width / 2.f, powerMeterRect.top - 24.f);
        window.draw(pwrTxt);

        sf::Text hintTxt;
        hintTxt.setFont(*font);
        hintTxt.setString(isDragging
            ? "Tarik mundur untuk atur tenaga | Lepas klik = Tembak | Klik Kanan / Dorong ke 0% = Batal"
            : "Gerakkan mouse (atau Panah Kiri/Kanan) untuk membidik | Klik Kiri & tarik mundur untuk memukul");
        hintTxt.setCharacterSize(13);
        hintTxt.setFillColor(isDragging ? sf::Color(250, 204, 21) : sf::Color(148, 163, 184));
        sf::FloatRect hb = hintTxt.getLocalBounds();
        hintTxt.setOrigin(hb.left + hb.width / 2.f, 0.f);
        hintTxt.setPosition(535.f, 615.f);
        window.draw(hintTxt);
    }
}