#include "../include/Game.h"
#include <iostream>
#include <fstream>
#include <cmath>

// ====================== PLAYER ======================
Player::Player(const sf::Texture& texture) {
    sprite.setTexture(texture);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f,
                     sprite.getLocalBounds().height / 2.0f);
}

void Player::move(double dx, double dy, float dt) {
    sprite.move(static_cast<float>(dx * speed * dt),
                static_cast<float>(dy * speed * dt));
}

void Player::triggerBong() {
    sprite.setScale(0.85f, 0.85f);
    animClock.restart();
    animating = true;
    animTime = 0.0f;
}

void Player::updateBong(float dt) {
    if (!animating) return;
    animTime += dt;
    float t = animTime / ANIM_DURATION;
    if (t >= 1.0f) { t = 1.0f; animating = false; }

    float bounce = std::sin(t * 3.14159f * 3.0f) * std::exp(-t * 4.0f);
    float scale = 0.85f + (1.0f - 0.85f) * t + 0.15f * bounce;
    sprite.setScale(scale, scale);
}

// ====================== BACKGROUND ======================
Background::Background(const sf::Texture& texture) {
    sprite.setTexture(texture);
    sprite.setScale(1920.0f / texture.getSize().x,
                    1080.0f / texture.getSize().y);
}

void UpgradeButton::scaling(const sf::Texture& texture,float scaleX,float scaleY){
    sprite.setTexture(texture);
    sprite.setScale(scaleX, scaleY);
}

// ====================== GAME ======================
Game::Game() {
    window.setFramerateLimit(147);

    // Load textures
    if (!backTexture.loadFromFile("assets/background.png") ||
        !playerTexture.loadFromFile("assets/player.png") ||
        !font.loadFromFile("assets/font.ttf")) {
        std::cerr << "Missing critical files!\n";
        std::exit(1);
    }

    // Upgrade button with fallback red square
    if (!upgradeTexture.loadFromFile("assets/upgrade.png")) {
        sf::Image img;
        img.create(120, 120, sf::Color(220, 20, 60));
        upgradeTexture.loadFromImage(img);
    }

    if(!grandpaTexture.loadFromFile("assets/grandpa.png")){
        sf::Image img1;
        img1.create(120, 120, sf::Color(150, 75, 0));
        grandpaTexture.loadFromImage(img1);
    }

    // Construct objects safely
    new (&bg) Background(backTexture);
    new (&player) Player(playerTexture);
    new (&upgradeButton) UpgradeButton(upgradeTexture, 10, 1);
    new (&Grandpa) UpgradeButton(grandpaTexture, 300, 1); 
    

    player.sprite.setPosition(960, 540);
    upgradeButton.sprite.setPosition(50, 500);
    upgradeButton.sprite.setScale(2.0f, 2.0f);
    Grandpa.sprite.setPosition(200, 500);
    Grandpa.scaling(grandpaTexture,0.4f,0.4f);

    // Reset upgrade every launch (for testing)
    cookiesPerClick = 1;
    upgradeButton.purchased = false;
    upgradeButton.sprite.setColor(sf::Color::White);

    // Sound
    if (clickBuffer.loadFromFile("click.wav")) {
        clickSound.setBuffer(clickBuffer);
        clickSound.setVolume(70.f);
    }

    // Text
    cookieText.setFont(font);
    cookieText.setCharacterSize(60);
    cookieText.setFillColor(sf::Color::White);
    cookieText.setOutlineColor(sf::Color::Black);
    cookieText.setOutlineThickness(3);
    cookieText.setPosition(760, 50);

    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::Yellow);
    fpsText.setPosition(10, 10);

    upText.setFont(font);
    upText.setCharacterSize(30);
    upText.setFillColor(sf::Color::White);
    upText.setPosition(upgradeButton.sprite.getPosition().x, upgradeButton.sprite.getPosition().y + upgradeButton.sprite.getGlobalBounds().height + 10);
    upText.setString("Upgrade: +1 Cookie/Click\nCost: 10 Cookies");

    loadGame();
}

void Game::loadGame() {
    std::ifstream f("save.txt");
    if (f.is_open()) { f >> cookies; f.close(); }
}

void Game::saveGame() {
    std::ofstream f("save.txt");
    if (f.is_open()) { f << cookies; f.close(); }
}

void Game::handleEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed ||
            (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)) {
            saveGame();
            window.close();
        }

        // THE BIG FIX: mapPixelToCoords makes clicks work in ANY window size
        if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);  // THIS LINE FIXES EVERYTHING

            // Click cookie
            if (player.sprite.getGlobalBounds().contains(worldPos)) {
                cookies += cookiesPerClick;
                player.triggerBong();
                if (clickBuffer.getDuration() > sf::seconds(0))
                    clickSound.play();
            }

            // Click upgrade button — WORKS IN WINDOWED, MAXIMIZED, RESIZED, EVERYTHING
            if (upgradeButton.sprite.getGlobalBounds().contains(worldPos)) {
                if (cookies >= upgradeButton.cost && 100 > costIncrement) {
                    cookies -= upgradeButton.cost;
                    cookiesPerClick += upgradeButton.cpsIncrease;
                    costIncrement += 10;
                    upgradeButton.cost += costIncrement;
                    upText.setString("Upgrade: +" + std::to_string(upgradeButton.cpsIncrease) + " Cookie/Click\nCost:"  + std::to_string(upgradeButton.cost) + " Cookies");
                }
                if( 100 <= costIncrement){
                    upgradeButton.sprite.setColor(sf::Color(80, 80, 80));upgradeButton.purchased = true; upText.setString("Done!");
                }
                
            }

        }
    }
}

void Game::update(float dt) {
    player.updateBong(dt);

    // Movement
    double dx = 0, dy = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dy -= 2;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dy += 2;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dx -= 2;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dx += 2;
    if (dx && dy) { dx *= 0.7071; dy *= 0.7071; }
    player.move(dx, dy, dt);

    // Screen wrap
    auto b = player.sprite.getGlobalBounds();
    auto p = player.sprite.getPosition();
    if (p.x > 1920)  player.sprite.setPosition(-b.width + 10, p.y);
    if (p.x < -b.width) player.sprite.setPosition(1910, p.y);
    if (p.y > 1080)  player.sprite.setPosition(p.x, -b.height + 10);
    if (p.y < -b.height) player.sprite.setPosition(p.x, 1070);

    cookieText.setString("Cookies: " + std::to_string(cookies) + " | Per Click: " + std::to_string(cookiesPerClick));

    // FPS
    frameCount++;
    fpsTimer += dt;
    if (fpsTimer >= 1.0f) {
        fpsText.setString("FPS: " + std::to_string(frameCount));
        frameCount = 0;
        fpsTimer = 0;
    }

    // Auto-save
    if (saveTimer.getElapsedTime().asSeconds() >= 25.0f) {
        saveGame();
        saveTimer.restart();
    }
}

void Game::render() {
    window.clear();
    window.draw(bg.sprite);
    window.draw(player.sprite);
    window.draw(upgradeButton.sprite);
    window.draw(Grandpa.sprite);
    window.draw(cookieText);
    window.draw(upText);
    window.draw(fpsText);
    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
        
    }
}